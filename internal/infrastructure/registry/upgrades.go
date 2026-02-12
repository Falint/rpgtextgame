// Package registry provides data registries for game entities.
// All data here EXACTLY matches C src/data/*.c files.
package registry

// UpgradeMaterial represents a required material for upgrade.
// EXACT MATCH: C src/data/upgrades.h MaterialRequirement struct
type UpgradeMaterial struct {
	ItemID   string
	Quantity int
}

// UpgradeRecipe defines how to upgrade a weapon.
// EXACT MATCH: C src/data/upgrades.h UpgradeRecipe struct
type UpgradeRecipe struct {
	WeaponID    string
	FromTier    int
	ToTier      int
	GoldCost    int
	Materials   []UpgradeMaterial
	DamageBonus int
}

// UpgradeRegistry holds all upgrade recipes.
type UpgradeRegistry struct {
	recipes map[string][]UpgradeRecipe // keyed by weapon_id
}

// NewUpgradeRegistry creates registry with all upgrades from C upgrades.c.
func NewUpgradeRegistry() *UpgradeRegistry {
	r := &UpgradeRegistry{
		recipes: make(map[string][]UpgradeRecipe),
	}
	r.loadRecipes()
	return r
}

// loadRecipes initializes all upgrade data.
// EXACT MATCH: C src/data/upgrades.c UPGRADE_DB[]
func (r *UpgradeRegistry) loadRecipes() {
	recipes := []UpgradeRecipe{
		// =========================================
		// IRON SWORD UPGRADES
		// =========================================
		{
			WeaponID: "iron_sword", FromTier: 1, ToTier: 2,
			GoldCost: 80, DamageBonus: 3,
			Materials: []UpgradeMaterial{
				{"goblin_fang", 3}, {"slime_gel", 2},
			},
		},
		{
			WeaponID: "iron_sword", FromTier: 2, ToTier: 3,
			GoldCost: 150, DamageBonus: 4,
			Materials: []UpgradeMaterial{
				{"goblin_fang", 5}, {"wolf_fang", 3},
			},
		},
		{
			WeaponID: "iron_sword", FromTier: 3, ToTier: 4,
			GoldCost: 300, DamageBonus: 5,
			Materials: []UpgradeMaterial{
				{"orc_tusk", 3}, {"bone_fragment", 5},
			},
		},
		{
			WeaponID: "iron_sword", FromTier: 4, ToTier: 5,
			GoldCost: 500, DamageBonus: 8,
			Materials: []UpgradeMaterial{
				{"legendary_ore", 1}, {"orc_tusk", 5},
			},
		},

		// =========================================
		// FIRE BLADE UPGRADES
		// =========================================
		{
			WeaponID: "fire_blade", FromTier: 1, ToTier: 2,
			GoldCost: 120, DamageBonus: 4,
			Materials: []UpgradeMaterial{
				{"fire_essence", 2}, {"ember_stone", 1},
			},
		},
		{
			WeaponID: "fire_blade", FromTier: 2, ToTier: 3,
			GoldCost: 250, DamageBonus: 5,
			Materials: []UpgradeMaterial{
				{"fire_essence", 4}, {"ember_stone", 3},
			},
		},
		{
			WeaponID: "fire_blade", FromTier: 3, ToTier: 4,
			GoldCost: 450, DamageBonus: 7,
			Materials: []UpgradeMaterial{
				{"fire_essence", 6}, {"dragon_fang", 1},
			},
		},
		{
			WeaponID: "fire_blade", FromTier: 4, ToTier: 5,
			GoldCost: 800, DamageBonus: 10,
			Materials: []UpgradeMaterial{
				{"dragon_scale", 2}, {"legendary_ore", 1},
			},
		},

		// =========================================
		// AQUA SABER UPGRADES
		// =========================================
		{
			WeaponID: "aqua_saber", FromTier: 1, ToTier: 2,
			GoldCost: 120, DamageBonus: 4,
			Materials: []UpgradeMaterial{
				{"water_essence", 2}, {"aqua_crystal", 1},
			},
		},
		{
			WeaponID: "aqua_saber", FromTier: 2, ToTier: 3,
			GoldCost: 250, DamageBonus: 5,
			Materials: []UpgradeMaterial{
				{"water_essence", 4}, {"aqua_crystal", 3},
			},
		},
		{
			WeaponID: "aqua_saber", FromTier: 3, ToTier: 4,
			GoldCost: 450, DamageBonus: 7,
			Materials: []UpgradeMaterial{
				{"water_essence", 6}, {"deep_sea_pearl", 1},
			},
		},
		{
			WeaponID: "aqua_saber", FromTier: 4, ToTier: 5,
			GoldCost: 800, DamageBonus: 10,
			Materials: []UpgradeMaterial{
				{"kraken_tentacle", 2}, {"legendary_ore", 1},
			},
		},

		// =========================================
		// EARTH CLEAVER UPGRADES
		// =========================================
		{
			WeaponID: "earth_cleaver", FromTier: 1, ToTier: 2,
			GoldCost: 120, DamageBonus: 4,
			Materials: []UpgradeMaterial{
				{"earth_essence", 2}, {"stone_core", 1},
			},
		},
		{
			WeaponID: "earth_cleaver", FromTier: 2, ToTier: 3,
			GoldCost: 250, DamageBonus: 5,
			Materials: []UpgradeMaterial{
				{"earth_essence", 4}, {"stone_core", 2},
			},
		},
		{
			WeaponID: "earth_cleaver", FromTier: 3, ToTier: 4,
			GoldCost: 450, DamageBonus: 7,
			Materials: []UpgradeMaterial{
				{"earth_essence", 6}, {"adamant_ore", 2},
			},
		},
		{
			WeaponID: "earth_cleaver", FromTier: 4, ToTier: 5,
			GoldCost: 800, DamageBonus: 10,
			Materials: []UpgradeMaterial{
				{"titan_heart", 1}, {"legendary_ore", 1},
			},
		},

		// =========================================
		// WIND CUTTER UPGRADES
		// =========================================
		{
			WeaponID: "wind_cutter", FromTier: 1, ToTier: 2,
			GoldCost: 100, DamageBonus: 3,
			Materials: []UpgradeMaterial{
				{"wind_essence", 2}, {"gale_feather", 2},
			},
		},
		{
			WeaponID: "wind_cutter", FromTier: 2, ToTier: 3,
			GoldCost: 220, DamageBonus: 4,
			Materials: []UpgradeMaterial{
				{"wind_essence", 4}, {"gale_feather", 4},
			},
		},
		{
			WeaponID: "wind_cutter", FromTier: 3, ToTier: 4,
			GoldCost: 400, DamageBonus: 6,
			Materials: []UpgradeMaterial{
				{"wind_essence", 6}, {"bat_wing", 10},
			},
		},
		{
			WeaponID: "wind_cutter", FromTier: 4, ToTier: 5,
			GoldCost: 700, DamageBonus: 9,
			Materials: []UpgradeMaterial{
				{"gale_feather", 10}, {"legendary_ore", 1},
			},
		},

		// =========================================
		// IRON AXE UPGRADES
		// =========================================
		{
			WeaponID: "iron_axe", FromTier: 1, ToTier: 2,
			GoldCost: 100, DamageBonus: 4,
			Materials: []UpgradeMaterial{
				{"wolf_pelt", 3}, {"bone_fragment", 3},
			},
		},
		{
			WeaponID: "iron_axe", FromTier: 2, ToTier: 3,
			GoldCost: 200, DamageBonus: 5,
			Materials: []UpgradeMaterial{
				{"orc_tusk", 2}, {"wolf_fang", 5},
			},
		},
		{
			WeaponID: "iron_axe", FromTier: 3, ToTier: 4,
			GoldCost: 400, DamageBonus: 7,
			Materials: []UpgradeMaterial{
				{"orc_tusk", 5}, {"stone_core", 2},
			},
		},
		{
			WeaponID: "iron_axe", FromTier: 4, ToTier: 5,
			GoldCost: 700, DamageBonus: 10,
			Materials: []UpgradeMaterial{
				{"adamant_ore", 3}, {"legendary_ore", 1},
			},
		},
	}

	// Index by weapon_id
	for _, recipe := range recipes {
		r.recipes[recipe.WeaponID] = append(r.recipes[recipe.WeaponID], recipe)
	}
}

// GetRecipe returns upgrade recipe for a weapon at given tier.
func (r *UpgradeRegistry) GetRecipe(weaponID string, fromTier int) *UpgradeRecipe {
	recipes, ok := r.recipes[weaponID]
	if !ok {
		return nil
	}

	for i := range recipes {
		if recipes[i].FromTier == fromTier {
			return &recipes[i]
		}
	}
	return nil
}

// GetAllRecipesForWeapon returns all upgrade paths for a weapon.
func (r *UpgradeRegistry) GetAllRecipesForWeapon(weaponID string) []UpgradeRecipe {
	return r.recipes[weaponID]
}

// GetUpgradeableWeaponIDs returns list of weapons that have upgrade paths.
func (r *UpgradeRegistry) GetUpgradeableWeaponIDs() []string {
	result := make([]string, 0, len(r.recipes))
	for id := range r.recipes {
		result = append(result, id)
	}
	return result
}
