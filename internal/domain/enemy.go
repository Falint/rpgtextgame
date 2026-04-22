package domain

import "math/rand" // Required for random gold roll within min/max range

// Enemy represents a combat opponent in the game.
// It holds all mutable state for a single battle encounter.
// Created from MonsterTemplate.ToEnemy() during battle initialization.
type Enemy struct {
	ID        string      // Unique identifier matching the MonsterTemplate ID
	Name      string      // Display name shown in battle UI
	Type      MonsterType // Difficulty category (Normal/Elite/Boss)
	Element   Element     // Elemental affinity affecting damage calculations
	MaxHP     int         // Maximum hit points — set from template BaseHP
	CurrentHP int         // Current hit points — decremented during combat
	Attack    int         // Base attack value used for damage calculation
	Defense   int         // Base defense value reducing incoming damage
	GoldMin   int         // Minimum gold rewarded on defeat
	GoldMax   int         // Maximum gold rewarded on defeat
}

// MonsterType categorizes enemy difficulty.
// Determines encounter behavior: stat scaling, loot quality, and escapability.
type MonsterType int

// Monster type constants using iota for auto-incrementing enum values.
const (
	MonsterNormal MonsterType = iota // Standard encounter, baseline stats, can be fled
	MonsterElite                     // Increased stats and better loot, can be fled
	MonsterBoss                      // Highest stats and rewards, CANNOT be escaped
)

// String returns the monster type name for display purposes.
// Used by battle screen to render [ELITE] and [BOSS] tags.
func (t MonsterType) String() string {
	switch t {
	case MonsterElite:
		return "Elite" // Mid-tier difficulty label
	case MonsterBoss:
		return "BOSS" // Highest difficulty label — uppercase for emphasis
	default:
		return "Normal" // Default label for standard encounters
	}
}

// IsAlive returns true if enemy has HP remaining.
// Used by BattleService to check if the battle should continue.
func (e *Enemy) IsAlive() bool {
	return e.CurrentHP > 0 // Enemy is alive as long as HP is positive
}

// TakeDamage applies damage to the enemy, considering its defense value.
// Returns true if the enemy died as a result of this damage.
// Uses the shared DefenseReductionRatio (0.5) and MinDamage (1) constants
// from player.go to ensure consistent damage calculation across all entities.
func (e *Enemy) TakeDamage(amount int) bool {
	// Calculate damage reduced by defense: defense absorbs 50% of its value
	reduced := amount - int(float64(e.Defense)*DefenseReductionRatio)
	// Enforce minimum damage of 1 to prevent zero-damage stalling
	if reduced < MinDamage {
		reduced = MinDamage
	}
	// Apply damage to current HP
	e.CurrentHP -= reduced
	// Clamp HP to zero — prevents negative HP display
	if e.CurrentHP < 0 {
		e.CurrentHP = 0
	}
	// Return true if enemy HP reached zero or below (defeated)
	return e.CurrentHP <= 0
}

// RollGold returns a random gold amount within the enemy's reward range.
// Called by BattleService.onEnemyDefeated() to determine gold reward.
func (e *Enemy) RollGold() int {
	// Guard: if max <= min, return min to avoid invalid rand.Intn(0) call
	if e.GoldMax <= e.GoldMin {
		return e.GoldMin
	}
	// Generate random value in [GoldMin, GoldMax] inclusive
	// rand.Intn(n) returns [0, n), so +1 makes the range inclusive
	return e.GoldMin + rand.Intn(e.GoldMax-e.GoldMin+1)
}

// GetAttack returns the enemy's attack value.
// Provides uniform access to attack stat matching the player's GetAttack() pattern.
func (e *Enemy) GetAttack() int {
	return e.Attack // Direct field access — enemies don't have weapon/buff modifiers
}

// GetElement returns the enemy's element.
// Used by BattleService.enemyTurn() for elemental damage modifier calculation.
func (e *Enemy) GetElement() Element {
	return e.Element // Direct field access for combat calculations
}
