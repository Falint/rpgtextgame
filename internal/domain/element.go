// Package domain contains pure business logic for TextRPG.
//
// DESIGN PRINCIPLES:
// - No external dependencies (no UI, no infrastructure)
// - All types are value types or have explicit ownership
// - No global state
// - Domain errors use ResultCode enum
package domain

// Element represents elemental affinity for damage calculations.
// Uses an int enum (iota) for type-safe, zero-allocation comparisons.
// Elements follow a rock-paper-scissors cycle: Fire > Wind > Earth > Water > Fire.
type Element int

// Element constants define the four elemental affinities plus None.
// iota auto-increments: None=0, Fire=1, Water=2, Earth=3, Wind=4.
const (
	ElementNone  Element = iota // No elemental affinity — neutral in all matchups
	ElementFire                 // Strong vs Wind, Weak vs Water
	ElementWater                // Strong vs Fire, Weak vs Earth
	ElementEarth                // Strong vs Water, Weak vs Wind
	ElementWind                 // Strong vs Earth, Weak vs Fire
)

// Element damage multipliers for rock-paper-scissors matchups.
// Named constants eliminate magic numbers from damage calculation logic.
const (
	ElementStrongMultiplier  = 2.0 // Attacker has advantage — doubles damage output
	ElementWeakMultiplier    = 0.5 // Attacker has disadvantage — halves damage output
	ElementNeutralMultiplier = 1.0 // No elemental interaction — damage unmodified
)

// String returns the display name of an element.
// Implements the fmt.Stringer interface for clean logging and UI rendering.
func (e Element) String() string {
	switch e { // Dispatch based on the element value
	case ElementFire:
		return "Fire" // Display string for fire element
	case ElementWater:
		return "Water" // Display string for water element
	case ElementEarth:
		return "Earth" // Display string for earth element
	case ElementWind:
		return "Wind" // Display string for wind element
	default:
		return "None" // Fallback for ElementNone and any unexpected value
	}
}

// DamageModifier returns the damage multiplier when attacking a target element.
//
// Rock-paper-scissors style:
// Fire > Wind > Earth > Water > Fire
//
// If either attacker or target has ElementNone, the modifier is neutral (1.0).
// This ensures non-elemental entities are neither advantaged nor disadvantaged.
func (e Element) DamageModifier(target Element) float64 {
	// Guard: if either side has no element, no modifier is applied.
	// This prevents non-elemental weapons/enemies from triggering the RPS system.
	if e == ElementNone || target == ElementNone {
		return ElementNeutralMultiplier // 1.0 — no change to damage
	}

	// Check each element's advantage/disadvantage matchup.
	// Each case returns immediately upon finding a match, avoiding fall-through.
	switch e {
	case ElementFire:
		if target == ElementWind {
			return ElementStrongMultiplier // Fire is strong against Wind (2.0x)
		}
		if target == ElementWater {
			return ElementWeakMultiplier // Fire is weak against Water (0.5x)
		}
	case ElementWater:
		if target == ElementFire {
			return ElementStrongMultiplier // Water is strong against Fire (2.0x)
		}
		if target == ElementEarth {
			return ElementWeakMultiplier // Water is weak against Earth (0.5x)
		}
	case ElementEarth:
		if target == ElementWater {
			return ElementStrongMultiplier // Earth is strong against Water (2.0x)
		}
		if target == ElementWind {
			return ElementWeakMultiplier // Earth is weak against Wind (0.5x)
		}
	case ElementWind:
		if target == ElementEarth {
			return ElementStrongMultiplier // Wind is strong against Earth (2.0x)
		}
		if target == ElementFire {
			return ElementWeakMultiplier // Wind is weak against Fire (0.5x)
		}
	}

	// Same-element attacks or unhandled cases result in neutral damage.
	return ElementNeutralMultiplier // 1.0 — no advantage or disadvantage
}
