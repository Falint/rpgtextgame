// Package registry provides read-only data registries for game entities.
// Mirrors the C data layer with all monsters, weapons, and items.
package registry

import (
	"math/rand" // Required for GetRandom() uniform monster selection

	"github.com/tenyom/textrpg-tui/internal/domain" // Domain types for MonsterType, Element, Enemy
)

// MonsterTemplate is an immutable monster definition loaded at startup.
// Used by BattleService to create mutable Enemy instances via ToEnemy().
type MonsterTemplate struct {
	ID        string             // Unique key (e.g., "slime", "dragon")
	Name      string             // Display name shown in battle UI
	Type      domain.MonsterType // Difficulty tier: Normal, Elite, or Boss
	Element   domain.Element     // Elemental affinity for damage calculations
	BaseHP    int                // Starting hit points for this monster
	BaseAtk   int                // Base attack power
	BaseDef   int                // Base defense value
	GoldMin   int                // Minimum gold reward on defeat
	GoldMax   int                // Maximum gold reward on defeat
	LootTable []LootEntry        // Possible item drops (NOTE: not yet processed in battle)
}

// LootEntry defines a single possible item drop from a monster.
// Used in loot table definitions — not yet processed by BattleService.
type LootEntry struct {
	ItemID     string  // Item registry ID of the droppable item
	DropChance float64 // Probability of dropping (0.0 to 1.0, e.g., 0.5 = 50%)
}

// ToEnemy creates a mutable domain.Enemy instance from this immutable template.
// The enemy starts at full HP (CurrentHP = BaseHP) and copies all stats.
// Called by BattleService.StartBattle() for each new combat encounter.
func (t *MonsterTemplate) ToEnemy() *domain.Enemy {
	return &domain.Enemy{
		ID:        t.ID,      // Copy identifier
		Name:      t.Name,    // Copy display name
		Type:      t.Type,    // Copy difficulty tier
		Element:   t.Element, // Copy element for damage modifiers
		MaxHP:     t.BaseHP,  // Set max HP from template
		CurrentHP: t.BaseHP,  // Start at full health
		Attack:    t.BaseAtk, // Copy attack power
		Defense:   t.BaseDef, // Copy defense value
		GoldMin:   t.GoldMin, // Copy min gold reward
		GoldMax:   t.GoldMax, // Copy max gold reward
	}
}

// MonsterRegistry is the read-only data store for all monster definitions.
// Supports lookup by ID, by type, and random selection for encounters.
type MonsterRegistry struct {
	monsters map[string]*MonsterTemplate               // All monsters indexed by ID
	byType   map[domain.MonsterType][]*MonsterTemplate // Monsters grouped by difficulty tier
}

// NewMonsterRegistry creates and initializes the monster registry.
// Populates both the by-ID and by-type indexes for efficient querying.
func NewMonsterRegistry() *MonsterRegistry {
	r := &MonsterRegistry{
		monsters: make(map[string]*MonsterTemplate),               // Initialize ID lookup
		byType:   make(map[domain.MonsterType][]*MonsterTemplate), // Initialize type grouping
	}
	r.loadMonsters() // Load all 10 monster definitions from hardcoded data
	return r
}

// loadMonsters initializes all monster data (matches C monsters.c).
func (r *MonsterRegistry) loadMonsters() {
	monsters := []*MonsterTemplate{
		// Normal monsters
		{
			ID: "slime", Name: "Slime", Type: domain.MonsterNormal,
			Element: domain.ElementWater, BaseHP: 30, BaseAtk: 8, BaseDef: 2,
			GoldMin: 10, GoldMax: 20,
			LootTable: []LootEntry{{ItemID: "slime_gel", DropChance: 0.5}},
		},
		{
			ID: "goblin", Name: "Goblin", Type: domain.MonsterNormal,
			Element: domain.ElementNone, BaseHP: 40, BaseAtk: 12, BaseDef: 4,
			GoldMin: 15, GoldMax: 30,
			LootTable: []LootEntry{{ItemID: "goblin_ear", DropChance: 0.3}},
		},
		{
			ID: "wolf", Name: "Wild Wolf", Type: domain.MonsterNormal,
			Element: domain.ElementWind, BaseHP: 35, BaseAtk: 15, BaseDef: 3,
			GoldMin: 12, GoldMax: 25,
			LootTable: []LootEntry{{ItemID: "wolf_pelt", DropChance: 0.4}},
		},
		{
			ID: "skeleton", Name: "Skeleton", Type: domain.MonsterNormal,
			Element: domain.ElementNone, BaseHP: 45, BaseAtk: 14, BaseDef: 6,
			GoldMin: 18, GoldMax: 35,
			LootTable: []LootEntry{{ItemID: "bone_fragment", DropChance: 0.6}},
		},
		{
			ID: "bat", Name: "Giant Bat", Type: domain.MonsterNormal,
			Element: domain.ElementWind, BaseHP: 25, BaseAtk: 10, BaseDef: 2,
			GoldMin: 8, GoldMax: 18,
			LootTable: []LootEntry{{ItemID: "bat_wing", DropChance: 0.45}},
		},

		// Elite monsters
		{
			ID: "goblin_chief", Name: "Goblin Chief", Type: domain.MonsterElite,
			Element: domain.ElementFire, BaseHP: 80, BaseAtk: 22, BaseDef: 10,
			GoldMin: 40, GoldMax: 70,
			LootTable: []LootEntry{
				{ItemID: "goblin_ear", DropChance: 1.0},
				{ItemID: "iron_ore", DropChance: 0.5},
			},
		},
		{
			ID: "dire_wolf", Name: "Dire Wolf", Type: domain.MonsterElite,
			Element: domain.ElementWind, BaseHP: 70, BaseAtk: 28, BaseDef: 8,
			GoldMin: 35, GoldMax: 60,
			LootTable: []LootEntry{
				{ItemID: "wolf_pelt", DropChance: 1.0},
				{ItemID: "wolf_fang", DropChance: 0.4},
			},
		},
		{
			ID: "flame_elemental", Name: "Flame Elemental", Type: domain.MonsterElite,
			Element: domain.ElementFire, BaseHP: 60, BaseAtk: 35, BaseDef: 5,
			GoldMin: 45, GoldMax: 80,
			LootTable: []LootEntry{{ItemID: "fire_essence", DropChance: 0.6}},
		},

		// Boss monsters
		{
			ID: "dragon", Name: "Ancient Dragon", Type: domain.MonsterBoss,
			Element: domain.ElementFire, BaseHP: 200, BaseAtk: 45, BaseDef: 20,
			GoldMin: 150, GoldMax: 300,
			LootTable: []LootEntry{
				{ItemID: "dragon_scale", DropChance: 1.0},
				{ItemID: "fire_essence", DropChance: 1.0},
			},
		},
		{
			ID: "lich", Name: "Lich King", Type: domain.MonsterBoss,
			Element: domain.ElementNone, BaseHP: 180, BaseAtk: 40, BaseDef: 15,
			GoldMin: 120, GoldMax: 250,
			LootTable: []LootEntry{
				{ItemID: "dark_crystal", DropChance: 1.0},
				{ItemID: "bone_fragment", DropChance: 1.0},
			},
		},
	}

	// Build both indexes: by-ID and by-type for efficient lookup
	for _, m := range monsters {
		r.monsters[m.ID] = m                           // Index by unique ID
		r.byType[m.Type] = append(r.byType[m.Type], m) // Group by difficulty tier
	}
}

// GetByID returns a monster template by its unique identifier.
// Returns nil if the ID doesn't match any monster.
func (r *MonsterRegistry) GetByID(id string) *MonsterTemplate {
	return r.monsters[id] // O(1) map lookup
}

// GetByType returns all monsters of the specified difficulty tier.
// Used for displaying available monsters or advanced encounter logic.
func (r *MonsterRegistry) GetByType(t domain.MonsterType) []*MonsterTemplate {
	return r.byType[t] // Returns slice of all monsters in tier
}

// GetRandom returns a uniformly random monster of the specified type.
// Used by BattleService.StartBattle() to select a random encounter.
// Returns nil if no monsters exist for the given type.
func (r *MonsterRegistry) GetRandom(t domain.MonsterType) *MonsterTemplate {
	list := r.byType[t] // Get all monsters of this type
	if len(list) == 0 {
		return nil // No monsters of this type registered
	}
	return list[rand.Intn(len(list))] // Uniform random selection
}

// GetAll returns all registered monsters regardless of type.
// Used for debug/admin display purposes.
func (r *MonsterRegistry) GetAll() []*MonsterTemplate {
	result := make([]*MonsterTemplate, 0, len(r.monsters)) // Pre-allocate with known size
	for _, m := range r.monsters {
		result = append(result, m) // Collect all templates
	}
	return result
}
