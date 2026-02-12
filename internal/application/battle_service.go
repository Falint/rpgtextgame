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

// BattleService handles combat logic.
type BattleService struct {
	player   *domain.Player
	enemy    *domain.Enemy
	monsters *registry.MonsterRegistry
	items    *registry.ItemRegistry
	state    BattleState
	log      []string
}

// NewBattleService creates a battle service.
func NewBattleService(player *domain.Player, monsters *registry.MonsterRegistry, items *registry.ItemRegistry) *BattleService {
	return &BattleService{
		player:   player,
		monsters: monsters,
		items:    items,
		state:    BattleNone,
		log:      make([]string, 0, 20),
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
	if b.state != BattlePlayerTurn {
		return false
	}

	slot := b.player.Inventory.GetSlot(slotIndex)
	if slot == nil || slot.Type != domain.SlotItem {
		return false
	}

	item := slot.Item
	if item.Category != domain.ItemConsumable {
		return false
	}

	// Apply effect
	switch item.ConsumableType {
	case domain.ConsumeHeal:
		b.player.Heal(item.Value)
		b.addLog(fmt.Sprintf("Used %s! Restored %d HP.", item.Name, item.Value))
	case domain.ConsumeBuffAtk:
		b.player.AddBuff(domain.BuffAttack, item.Value, item.Duration)
		b.addLog(fmt.Sprintf("Used %s! ATK +%d%% for %d turns.", item.Name, item.Value, item.Duration))
	case domain.ConsumeBuffDef:
		b.player.AddBuff(domain.BuffDefense, item.Value, item.Duration)
		b.addLog(fmt.Sprintf("Used %s! DEF +%d%% for %d turns.", item.Name, item.Value, item.Duration))
	case domain.ConsumeFullRestore:
		b.player.CurrentHP = b.player.MaxHP
		b.addLog(fmt.Sprintf("Used %s! Fully restored!", item.Name))
	}

	// Remove item
	b.player.Inventory.RemoveItem(slotIndex, 1)

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
	if len(b.log) > 20 {
		b.log = b.log[1:]
	}
}
