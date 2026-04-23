// Package application provides use-case services that orchestrate domain logic.
//
// DESIGN: Services in this package coordinate multi-entity operations by composing
// domain methods with infrastructure registry lookups. They contain no UI logic
// and no direct data access — all data comes through injected dependencies.
package application

import (
	"fmt"       // Sprintf for formatting battle log messages
	"math/rand" // Random number generation for critical hits and escape rolls

	"github.com/tenyom/textrpg-tui/internal/domain"                  // Domain entities and types
	"github.com/tenyom/textrpg-tui/internal/infrastructure/registry" // Monster data source
)

// Combat balance constants.
// Named constants prevent magic numbers and make tuning visible.
const (
	CriticalHitChance     = 0.10 // 10% chance of dealing critical damage each attack
	CriticalHitMultiplier = 1.5  // Critical hits deal 150% of calculated damage
	EscapeChance          = 0.5  // 50% base chance to escape from non-boss enemies
	battleLogCapacity     = 20   // Maximum number of log entries before oldest is dropped
)

// BattleState tracks the current phase of combat.
// State machine transitions: None → PlayerTurn → EnemyTurn → PlayerTurn ... → Won/Lost/Escaped
type BattleState int

// Battle state constants.
const (
	BattleNone       BattleState = iota // No battle active — default/idle state
	BattlePlayerTurn                    // Awaiting player action (Attack/Item/Run)
	BattleEnemyTurn                     // Enemy is attacking (auto-resolved)
	BattleWon                           // Player defeated the enemy
	BattleLost                          // Enemy defeated the player
	BattleEscaped                       // Player successfully escaped
)

// BattleService orchestrates turn-based combat.
// Manages the state machine, damage calculations, and reward distribution.
// Delegates item effects to ItemService for centralized processing.
type BattleService struct {
	player          *domain.Player            // Mutable player reference
	monsterRegistry *registry.MonsterRegistry // Read-only monster data source
	itemService     *ItemService              // Shared consumable effect processor

	// Battle state
	currentEnemy *domain.Enemy // Active enemy for current battle
	state        BattleState   // Current state machine position
	logs         []string      // Combat log messages for UI display
	turnCount    int           // Number of complete rounds elapsed
	goldEarned   int           // Gold rewarded on victory (0 if not won yet)
}

// NewBattleService creates battle service with injected dependencies.
// ItemService is shared with InventoryScreen to ensure consistent item effects.
func NewBattleService(
	player *domain.Player,
	monsterReg *registry.MonsterRegistry,
	itemService *ItemService,
) *BattleService {
	return &BattleService{
		player:          player,                               // Store player reference for combat mutations
		monsterRegistry: monsterReg,                           // Store registry for enemy lookup
		itemService:     itemService,                          // Store shared item processing service
		state:           BattleNone,                           // Start with no active battle
		logs:            make([]string, 0, battleLogCapacity), // Pre-allocate log buffer
	}
}

// StartBattle initializes combat against a random monster of the given type.
// Resets all battle state and transitions to PlayerTurn.
func (bs *BattleService) StartBattle(monsterType domain.MonsterType) {
	// Get a random monster template from the registry and create a mutable enemy
	template := bs.monsterRegistry.GetRandom(monsterType)
	bs.currentEnemy = template.ToEnemy() // Convert template → mutable enemy instance

	// Reset all battle state for the new encounter
	bs.state = BattlePlayerTurn // Player acts first
	bs.logs = bs.logs[:0]       // Clear log buffer (reuse underlying array)
	bs.turnCount = 0            // Reset turn counter
	bs.goldEarned = 0           // Reset reward tracker

	// Add initial battle log entry
	bs.addLog(fmt.Sprintf("A wild %s appears!", bs.currentEnemy.Name))

	// Show elemental type for non-None elements
	if bs.currentEnemy.Element != domain.ElementNone {
		bs.addLog(fmt.Sprintf("Element: %s", bs.currentEnemy.Element))
	}
}

// Attack executes player's attack action.
// Calculates damage with weapon bonus, elemental modifier, and critical hit roll.
// After player attack, triggers enemy turn if enemy survives.
func (bs *BattleService) Attack() {
	// Guard: only process attacks during player turn
	if bs.state != BattlePlayerTurn {
		return
	}

	// Step 1: Calculate base damage from player's total attack (base + weapon + buffs)
	baseDamage := bs.player.GetAttack()

	// Step 2: Apply elemental damage modifier (1.0x, 2.0x, or 0.5x)
	var elementMod float64 = 1.0 // Default neutral modifier
	if bs.player.EquippedWeapon != nil {
		// Get weapon's element matchup against enemy's element
		elementMod = bs.player.EquippedWeapon.Element.DamageModifier(bs.currentEnemy.GetElement())
	}
	damage := int(float64(baseDamage) * elementMod) // Apply element multiplier

	// Step 3: Roll for critical hit (10% chance for 1.5x damage)
	isCrit := rand.Float64() < CriticalHitChance
	if isCrit {
		damage = int(float64(damage) * CriticalHitMultiplier) // Apply crit multiplier
	}

	// Step 4: Apply damage to enemy (enemy.TakeDamage handles defense reduction)
	killed := bs.currentEnemy.TakeDamage(damage)

	// Step 5: Build log message with damage details
	logMsg := fmt.Sprintf("You attack %s for %d damage!", bs.currentEnemy.Name, damage)
	if isCrit {
		logMsg = "CRITICAL HIT! " + logMsg // Prepend crit indicator
	}
	// Add elemental effectiveness messages
	if elementMod > 1.0 {
		logMsg += " (Super effective!)" // 2.0x element advantage
	} else if elementMod < 1.0 {
		logMsg += " (Not very effective...)" // 0.5x element disadvantage
	}
	bs.addLog(logMsg) // Write to battle log

	// Step 6: Handle battle outcome
	if killed {
		bs.onEnemyDefeated() // Process victory rewards
	} else {
		bs.enemyTurn() // Enemy attacks back
	}
}

// UseItem attempts to use a consumable from inventory during battle.
// Delegates effect processing to ItemService for consistent behavior.
// On success, removes the item and advances to enemy turn.
func (bs *BattleService) UseItem(slotIndex int) {
	// Guard: ensure we're in player turn state
	if bs.state != BattlePlayerTurn {
		return
	}

	// Step 1: Validate the inventory slot
	slot := bs.player.Inventory.GetSlot(slotIndex)
	if slot == nil || slot.Type != domain.SlotItem {
		bs.addLog("Invalid item slot!") // Slot doesn't exist or isn't an item
		return
	}

	// Step 2: Delegate effect processing to ItemService (centralized logic)
	// This ensures identical validation and effects whether used in battle or inventory.
	result := bs.itemService.UseConsumable(bs.player, slot.Item)

	// Step 3: Handle result
	if result.Success {
		bs.addLog(result.Message) // Log the effect message (e.g., "Healed 30 HP!")
		// Remove one unit from inventory — only after confirming effect succeeded
		bs.player.Inventory.RemoveItem(slotIndex, 1)
		// Advance to enemy turn — using an item consumes the player's action
		bs.enemyTurn()
	} else {
		// Item use failed (e.g., HP already full) — no turn consumed
		bs.addLog(result.Error)
	}
}

// TryEscape attempts to flee from battle.
// Bosses cannot be escaped. Non-boss enemies have 50% escape chance.
func (bs *BattleService) TryEscape() {
	// Guard: only allow escape during player turn
	if bs.state != BattlePlayerTurn {
		return
	}

	// Boss encounters cannot be fled — design decision from C version
	if bs.currentEnemy.Type == domain.MonsterBoss {
		bs.addLog("Cannot escape from a Boss battle!") // Display reason in log
		return                                         // No turn consumed
	}

	// Roll for escape success (50% chance)
	if rand.Float64() < EscapeChance {
		bs.state = BattleEscaped // Transition to escaped state
		bs.addLog("You escaped successfully!")
	} else {
		bs.addLog("Failed to escape!") // Display failure message
		bs.enemyTurn()                 // Failed escape consumes turn — enemy attacks
	}
}

// enemyTurn executes enemy attack after player action.
// Increments turn counter and ticks buff durations.
func (bs *BattleService) enemyTurn() {
	bs.state = BattleEnemyTurn // Update state to enemy turn

	// Calculate enemy damage (no weapon/buff system for enemies — raw attack only)
	damage := bs.currentEnemy.GetAttack()

	// Apply damage to player (player.TakeDamage handles defense reduction)
	playerDied := bs.player.TakeDamage(damage)
	bs.addLog(fmt.Sprintf("%s attacks you for %d damage!", bs.currentEnemy.Name, damage))

	// Check for player death
	if playerDied {
		bs.state = BattleLost // Transition to defeat state
		bs.addLog("You have been defeated...")
		return
	}

	// Complete the full round — tick buffs and advance turn counter
	bs.turnCount++        // Increment round counter
	bs.player.TickBuffs() // Decrement buff durations, remove expired buffs

	// Return control to player for next action
	bs.state = BattlePlayerTurn
}

// onEnemyDefeated handles victory processing.
// Rolls gold reward and adds it to player's balance.
func (bs *BattleService) onEnemyDefeated() {
	bs.state = BattleWon // Transition to victory state

	// Roll random gold within enemy's [GoldMin, GoldMax] range
	gold := bs.currentEnemy.RollGold()
	bs.goldEarned = gold // Store for UI display

	// Add gold to player balance
	bs.player.AddGold(gold)

	// Log victory and reward
	bs.addLog(fmt.Sprintf("%s has been defeated!", bs.currentEnemy.Name))
	bs.addLog(fmt.Sprintf("You earned %d gold!", gold))

	// TODO: Implement loot drops from MonsterTemplate.LootTable
	// This would involve: checking drop chances, creating items from templates,
	// adding to inventory, and logging drops.
}

// addLog appends message to combat log.
// If the log exceeds capacity, the oldest entry is dropped (FIFO behavior).
func (bs *BattleService) addLog(msg string) {
	bs.logs = append(bs.logs, msg) // Append new message
	// Trim oldest messages if capacity exceeded
	if len(bs.logs) > battleLogCapacity {
		bs.logs = bs.logs[1:] // Remove first element (oldest message)
	}
}

// State returns current battle state.
// Used by BattleScreen to determine which mode/UI to display.
func (bs *BattleService) State() BattleState {
	return bs.state
}

// Enemy returns current enemy.
// Used by BattleScreen to render enemy status (name, HP, element).
func (bs *BattleService) Enemy() *domain.Enemy {
	return bs.currentEnemy
}

// Logs returns battle log.
// Used by BattleScreen to render combat history.
func (bs *BattleService) Logs() []string {
	return bs.logs
}

// TurnCount returns current turn number.
// Displayed in battle UI as "Turn: N".
func (bs *BattleService) TurnCount() int {
	return bs.turnCount
}

// GoldEarned returns gold earned from last victory.
// Displayed in victory result screen.
func (bs *BattleService) GoldEarned() int {
	return bs.goldEarned
}
