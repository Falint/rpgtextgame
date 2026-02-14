// Package domain contains pure business logic for TextRPG.
//
// DESIGN PRINCIPLES:
// - No external dependencies (no UI, no infrastructure)
// - All types are value types or have explicit ownership
// - No global state
// - Domain errors use ResultCode enum
package domain

// Element represents elemental affinity for damage calculations.
type Element int

const (
	ElementNone  Element = iota
	ElementFire          // Strong vs Wind, Weak vs Water
	ElementWater         // Strong vs Fire, Weak vs Earth
	ElementEarth         // Strong vs Water, Weak vs Wind
	ElementWind          // Strong vs Earth, Weak vs Fire
)

// Element damage multipliers for rock-paper-scissors matchups.
const (
	ElementStrongMultiplier  = 2.0 // Attacker has advantage
	ElementWeakMultiplier    = 0.5 // Attacker has disadvantage
	ElementNeutralMultiplier = 1.0 // No elemental interaction
)

// String returns the display name of an element.
func (e Element) String() string {
	switch e {
	case ElementFire:
		return "Fire"
	case ElementWater:
		return "Water"
	case ElementEarth:
		return "Earth"
	case ElementWind:
		return "Wind"
	default:
		return "None"
	}
}

// DamageModifier returns the damage multiplier when attacking a target element.
//
// Rock-paper-scissors style:
// Fire > Wind > Earth > Water > Fire
func (e Element) DamageModifier(target Element) float64 {
	if e == ElementNone || target == ElementNone {
		return ElementNeutralMultiplier
	}

	switch e {
	case ElementFire:
		if target == ElementWind {
			return ElementStrongMultiplier
		}
		if target == ElementWater {
			return ElementWeakMultiplier
		}
	case ElementWater:
		if target == ElementFire {
			return ElementStrongMultiplier
		}
		if target == ElementEarth {
			return ElementWeakMultiplier
		}
	case ElementEarth:
		if target == ElementWater {
			return ElementStrongMultiplier
		}
		if target == ElementWind {
			return ElementWeakMultiplier
		}
	case ElementWind:
		if target == ElementEarth {
			return ElementStrongMultiplier
		}
		if target == ElementFire {
			return ElementWeakMultiplier
		}
	}

	return ElementNeutralMultiplier
}
