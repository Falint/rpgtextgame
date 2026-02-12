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
		return 1.0
	}

	// Strong matchups (2x damage)
	switch e {
	case ElementFire:
		if target == ElementWind {
			return 2.0
		}
		if target == ElementWater {
			return 0.5
		}
	case ElementWater:
		if target == ElementFire {
			return 2.0
		}
		if target == ElementEarth {
			return 0.5
		}
	case ElementEarth:
		if target == ElementWater {
			return 2.0
		}
		if target == ElementWind {
			return 0.5
		}
	case ElementWind:
		if target == ElementEarth {
			return 2.0
		}
		if target == ElementFire {
			return 0.5
		}
	}

	return 1.0
}
