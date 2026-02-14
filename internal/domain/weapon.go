package domain

import "fmt"

// Weapon represents an equippable weapon.
type Weapon struct {
	ID          string
	Name        string
	Description string
	Type        WeaponType
	Element     Element
	BaseDamage  int
	Tier        int
	MaxTier     int
}

// WeaponType categorizes weapons.
type WeaponType int

const (
	WeaponFist WeaponType = iota
	WeaponSword
	WeaponAxe
	WeaponStaff
	WeaponBow
	WeaponDagger
)

// TierBonusPerLevel is the fractional damage increase per tier above 1.
const TierBonusPerLevel = 0.15

// String returns the weapon type name.
func (t WeaponType) String() string {
	switch t {
	case WeaponSword:
		return "Sword"
	case WeaponAxe:
		return "Axe"
	case WeaponStaff:
		return "Staff"
	case WeaponBow:
		return "Bow"
	case WeaponDagger:
		return "Dagger"
	default:
		return "Fist"
	}
}

// Damage returns the total damage including tier bonus.
func (w *Weapon) Damage() int {
	tierMult := 1.0 + (float64(w.Tier-1) * TierBonusPerLevel)
	return int(float64(w.BaseDamage) * tierMult)
}

// CanUpgrade returns true if weapon can be upgraded.
func (w *Weapon) CanUpgrade() bool {
	return w.Tier < w.MaxTier
}

// DisplayName returns name with tier indicator.
func (w *Weapon) DisplayName() string {
	if w.Tier > 1 {
		return fmt.Sprintf("%s +%d", w.Name, w.Tier-1)
	}
	return w.Name
}
