// Package application provides game services and orchestration.
package application

import (
	"fmt"
	"math/rand"

	"github.com/tenyom/textrpg-tui/internal/domain"
	"github.com/tenyom/textrpg-tui/internal/infrastructure/registry"
)

// BattleState represents current battle status.
type BattleState int

const (
	BattleNone BattleState = iota
	BattlePlayerTurn
	BattleEnemyTurn
	BattleWon
	BattleLost
	BattleEscaped
)

// battleLogCapacity is the maximum number of log entries retained in memory.
const battleLogCapacity = 20

// BattleService handles combat logic.
// It manages turn order, damage calculations, and victory/defeat conditions.
// Item effect application is delegated to the centralized ItemService.
type BattleService struct {
	player      *domain.Player
	enemy       *domain.Enemy
	monsters    *registry.MonsterRegistry
	items       *registry.ItemRegistry
	itemService *ItemService // Centralized item effect processor
	state       BattleState
	log         []string
}

// NewBattleService creates a battle service.
// itemService is the shared, centralized item effect processor.
func NewBattleService(
	player *domain.Player,
	monsters *registry.MonsterRegistry,
	items *registry.ItemRegistry,
	itemService *ItemService,
) *BattleService {
	return &BattleService{
		player:      player,
		monsters:    monsters,
		items:       items,
		itemService: itemService,
		state:       BattleNone,
		log:         make([]string, 0, battleLogCapacity),
	}
}

// StartBattle begins a new battle with random enemy of given type.
func (b *BattleService) StartBattle(monsterType domain.MonsterType) bool {
	template := b.monsters.GetRandom(monsterType)
	if template == nil {
		return false
	}

	b.enemy = template.ToEnemy()
	b.state = BattlePlayerTurn
	b.log = b.log[:0]
	b.addLog(fmt.Sprintf("A wild %s appears!", b.enemy.Name))

	return true
}

// GetEnemy returns current enemy.
func (b *BattleService) GetEnemy() *domain.Enemy {
	return b.enemy
}

// GetState returns current battle state.
func (b *BattleService) GetState() BattleState {
	return b.state
}

// GetLog returns battle log.
func (b *BattleService) GetLog() []string {
	return b.log
}

// Attack performs player attack on enemy.
func (b *BattleService) Attack() {
	if b.state != BattlePlayerTurn || b.enemy == nil {
		return
	}

	// Calculate damage
	atk := b.player.GetAttack()

	// Element modifier
	var modifier float64 = 1.0
	if b.player.EquippedWeapon != nil {
		modifier = b.player.EquippedWeapon.Element.DamageModifier(b.enemy.Element)
	}

	// Critical hit (10% chance, 1.5x damage)
	crit := rand.Float64() < 0.1
	if crit {
		atk = int(float64(atk) * 1.5)
	}

	damage := int(float64(atk) * modifier)
	if damage < 1 {
		damage = 1
	}

	// Apply damage
	killed := b.enemy.TakeDamage(damage)

	// Log
	if crit {
		b.addLog(fmt.Sprintf("CRITICAL HIT! You deal %d damage!", damage))
	} else {
		b.addLog(fmt.Sprintf("You deal %d damage!", damage))
	}

	if modifier > 1.0 {
		b.addLog("It's super effective!")
	} else if modifier < 1.0 {
		b.addLog("It's not very effective...")
	}

	if killed {
		b.onEnemyDefeated()
	} else {
		b.state = BattleEnemyTurn
		b.enemyTurn()
	}
}

// UseItem uses a consumable from inventory.
func (b *BattleService) UseItem(slotIndex int) bool {
	// Guard: only allow item usage during player's turn
	if b.state != BattlePlayerTurn {
		return false
	}

	// Validate slot contains a consumable item
	slot := b.player.Inventory.GetSlot(slotIndex)
	if slot == nil || slot.Type != domain.SlotItem {
		return false
	}
	if slot.Item.Category != domain.ItemConsumable {
		return false
	}

	// Delegate effect application to the centralized ItemService.
	// This ensures consistent behavior between battle and inventory usage.
	result := b.itemService.UseConsumable(slot.Item)

	if !result.Success {
		// Item effect was rejected (e.g., HP already full).
		// Log the reason but do NOT consume the item or end the turn.
		b.addLog(result.Error)
		return false
	}

	// Effect applied successfully — log the result message
	b.addLog(result.Message)

	// Remove the consumed item from the inventory slot
	b.player.Inventory.RemoveItem(slotIndex, 1)

	// End player's turn and trigger enemy response
	b.state = BattleEnemyTurn
	b.enemyTurn()
	return true
}

// TryEscape attempts to flee from battle.
func (b *BattleService) TryEscape() bool {
	if b.state != BattlePlayerTurn || b.enemy == nil {
		return false
	}

	// Boss cannot be escaped
	if b.enemy.Type == domain.MonsterBoss {
		b.addLog("Cannot escape from a BOSS!")
		return false
	}

	// 50% escape chance
	if rand.Float64() < 0.5 {
		b.addLog("Got away safely!")
		b.state = BattleEscaped
		return true
	}

	b.addLog("Couldn't escape!")
	b.state = BattleEnemyTurn
	b.enemyTurn()
	return false
}

// enemyTurn performs enemy attack.
func (b *BattleService) enemyTurn() {
	if b.enemy == nil || !b.enemy.IsAlive() {
		return
	}

	// Calculate damage
	atk := b.enemy.Attack
	modifier := b.enemy.Element.DamageModifier(domain.ElementNone)

	damage := int(float64(atk) * modifier)
	if damage < 1 {
		damage = 1
	}

	// Apply to player
	killed := b.player.TakeDamage(damage)

	b.addLog(fmt.Sprintf("%s deals %d damage to you!", b.enemy.Name, damage))

	if killed {
		b.addLog("You have been defeated...")
		b.state = BattleLost
	} else {
		// Tick buffs at end of full turn
		b.player.TickBuffs()
		b.state = BattlePlayerTurn
	}
}

// onEnemyDefeated handles victory rewards.
func (b *BattleService) onEnemyDefeated() {
	b.addLog(fmt.Sprintf("Defeated %s!", b.enemy.Name))

	// Gold reward
	gold := b.enemy.RollGold()
	b.player.AddGold(gold)
	b.addLog(fmt.Sprintf("Obtained %d gold!", gold))

	// Loot drops would go here
	// (simplified - full loot table processing would use registry)

	b.state = BattleWon
}

// EndBattle cleans up battle state.
func (b *BattleService) EndBattle() {
	b.enemy = nil
	b.state = BattleNone
}

// IsBattleOver returns true if battle ended.
func (b *BattleService) IsBattleOver() bool {
	return b.state == BattleWon || b.state == BattleLost || b.state == BattleEscaped
}

func (b *BattleService) addLog(msg string) {
	b.log = append(b.log, msg)
	if len(b.log) > battleLogCapacity {
		b.log = b.log[1:]
	}
}
