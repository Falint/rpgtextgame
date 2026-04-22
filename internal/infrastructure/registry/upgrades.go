// Package registry provides read-only data registries for game entities.
// All data definitions EXACTLY match C src/data/*.c files for parity.
package registry

// UpgradeMaterial represents a single material requirement for a weapon upgrade.
// Each upgrade recipe requires one or more materials in specific quantities.
type UpgradeMaterial struct {
	ItemID   string // Item registry ID of the required material (e.g., "goblin_fang")
	Quantity int    // Number of units needed for this upgrade step
}

// UpgradeRecipe defines the full requirements to upgrade a weapon by one tier.
// Each recipe specifies the source tier, target tier, gold cost, required
// materials, and the base damage bonus gained from the upgrade.
type UpgradeRecipe struct {
	WeaponID    string            // Weapon ID this recipe applies to
	FromTier    int               // Current tier required (e.g., 1 for first upgrade)
	ToTier      int               // Resulting tier after upgrade (FromTier + 1)
	GoldCost    int               // Gold required to perform the upgrade
	Materials   []UpgradeMaterial // List of materials and quantities needed
	DamageBonus int               // Base damage increase applied on upgrade
}

// UpgradeRegistry is the read-only data store for all weapon upgrade recipes.
// Recipes are indexed by weapon ID for O(1) lookup during upgrade operations.
type UpgradeRegistry struct {
	recipes map[string][]UpgradeRecipe // All recipes grouped by weapon ID
}

// NewUpgradeRegistry creates and initializes the upgrade registry.
// Loads all 24 upgrade recipes (6 weapons × 4 tier transitions each).
func NewUpgradeRegistry() *UpgradeRegistry {
	r := &UpgradeRegistry{
		recipes: make(map[string][]UpgradeRecipe), // Initialize recipe lookup map
	}
	r.loadRecipes() // Populate all upgrade paths from hardcoded data
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

	// Build index: group all recipes by their weapon ID for efficient lookup
	for _, recipe := range recipes {
		r.recipes[recipe.WeaponID] = append(r.recipes[recipe.WeaponID], recipe)
	}
}

// GetRecipe returns the upgrade recipe for a specific weapon at a specific tier.
// Used by UpgradeScreen to check requirements and perform upgrades.
// Returns nil if no recipe exists for the given weapon/tier combination.
func (r *UpgradeRegistry) GetRecipe(weaponID string, fromTier int) *UpgradeRecipe {
	recipes, ok := r.recipes[weaponID] // Lookup all recipes for this weapon
	if !ok {
		return nil // No upgrade path exists for this weapon
	}

	// Linear search for the specific tier transition
	for i := range recipes {
		if recipes[i].FromTier == fromTier {
			return &recipes[i] // Return pointer to matching recipe
		}
	}
	return nil // No recipe for this tier (weapon may be at max tier)
}

// GetAllRecipesForWeapon returns the full upgrade path for a weapon.
// Returns recipes for all tier transitions (e.g., 1→2, 2→3, 3→4, 4→5).
func (r *UpgradeRegistry) GetAllRecipesForWeapon(weaponID string) []UpgradeRecipe {
	return r.recipes[weaponID] // Returns nil slice if weapon has no upgrades
}

// GetUpgradeableWeaponIDs returns all weapon IDs that have at least one upgrade recipe.
// Used by UpgradeScreen to determine which weapons can potentially be upgraded.
func (r *UpgradeRegistry) GetUpgradeableWeaponIDs() []string {
	result := make([]string, 0, len(r.recipes)) // Pre-allocate with known capacity
	for id := range r.recipes {
		result = append(result, id) // Collect all weapon IDs with upgrade paths
	}
	return result
}
