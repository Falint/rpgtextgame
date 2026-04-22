package domain

import "fmt" // Required for Sprintf in DisplayName formatting

// Weapon represents an equippable weapon.
// Created from WeaponTemplate.ToWeapon() in the infrastructure layer.
// Mutable fields (Tier, BaseDamage) change during upgrades.
type Weapon struct {
	ID          string     // Unique identifier matching WeaponTemplate ID
	Name        string     // Base display name (e.g., "Iron Sword")
	Description string     // Flavor text for shop/details display
	Type        WeaponType // Categorization (Sword, Axe, Staff, Bow, Dagger)
	Element     Element    // Elemental affinity for damage modifier calculations
	BaseDamage  int        // Base damage value before tier multiplication
	Tier        int        // Current upgrade tier (1 = base, increases on upgrade)
	MaxTier     int        // Maximum achievable tier (typically 5)
}

// WeaponType categorizes weapons.
// Used for display purposes and potential future type-specific mechanics.
type WeaponType int

// Weapon type constants using iota for auto-incrementing enum.
const (
	WeaponFist   WeaponType = iota // Bare hands — default when no weapon equipped
	WeaponSword                    // Balanced weapon — standard damage
	WeaponAxe                      // Heavy weapon — high damage
	WeaponStaff                    // Magic weapon — element-focused
	WeaponBow                      // Ranged weapon — moderate damage
	WeaponDagger                   // Fast weapon — lower damage
)

// TierBonusPerLevel is the fractional damage increase per tier above 1.
// Each tier above 1 adds 15% to the base damage multiplicatively.
// Example: Tier 3 → 1.0 + (3-1) * 0.15 = 1.30x multiplier
const TierBonusPerLevel = 0.15

// String returns the weapon type name.
// Implements fmt.Stringer for display in status and detail screens.
func (t WeaponType) String() string {
	switch t {
	case WeaponSword:
		return "Sword" // Standard melee weapon
	case WeaponAxe:
		return "Axe" // Heavy melee weapon
	case WeaponStaff:
		return "Staff" // Magic-focused weapon
	case WeaponBow:
		return "Bow" // Ranged weapon
	case WeaponDagger:
		return "Dagger" // Fast melee weapon
	default:
		return "Fist" // Fallback for WeaponFist
	}
}

// Damage returns the total damage including tier bonus.
// Formula: BaseDamage * (1.0 + (Tier - 1) * TierBonusPerLevel)
// At Tier 1: multiplier = 1.0 (no bonus)
// At Tier 5: multiplier = 1.0 + 4 * 0.15 = 1.60 (60% bonus)
func (w *Weapon) Damage() int {
	// Calculate tier multiplier: starts at 1.0, increases 15% per tier
	tierMult := 1.0 + (float64(w.Tier-1) * TierBonusPerLevel)
	// Apply multiplier to base damage and truncate to integer
	return int(float64(w.BaseDamage) * tierMult)
}

// CanUpgrade returns true if weapon can be upgraded.
// Checked by upgrade screen to filter weapons eligible for upgrade.
func (w *Weapon) CanUpgrade() bool {
	return w.Tier < w.MaxTier // Can upgrade if current tier is below maximum
}

// DisplayName returns name with tier indicator.
// Shows "+N" suffix for upgraded weapons (e.g., "Iron Sword +2" at Tier 3).
// Tier 1 weapons show the base name without any suffix.
func (w *Weapon) DisplayName() string {
	if w.Tier > 1 {
		// Format with upgrade indicator: weapon name + tier offset
		return fmt.Sprintf("%s +%d", w.Name, w.Tier-1)
	}
	return w.Name // Base tier — no suffix needed
}
