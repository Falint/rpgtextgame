// Package registry provides data registries for game entities.
// Mirrors the C data layer with all monsters, weapons, and items.
package registry

import (
	"math/rand"

	"github.com/tenyom/textrpg-tui/internal/domain"
)

// MonsterTemplate is a read-only monster definition.
type MonsterTemplate struct {
	ID        string
	Name      string
	Type      domain.MonsterType
	Element   domain.Element
	BaseHP    int
	BaseAtk   int
	BaseDef   int
	GoldMin   int
	GoldMax   int
	LootTable []LootEntry
}

// LootEntry defines a possible item drop.
type LootEntry struct {
	ItemID     string
	DropChance float64
}

// ToEnemy creates an Enemy instance from template.
func (t *MonsterTemplate) ToEnemy() *domain.Enemy {
	return &domain.Enemy{
		ID:        t.ID,
		Name:      t.Name,
		Type:      t.Type,
		Element:   t.Element,
		MaxHP:     t.BaseHP,
		CurrentHP: t.BaseHP,
		Attack:    t.BaseAtk,
		Defense:   t.BaseDef,
		GoldMin:   t.GoldMin,
		GoldMax:   t.GoldMax,
	}
}

// MonsterRegistry holds all monster templates.
type MonsterRegistry struct {
	monsters map[string]*MonsterTemplate
	byType   map[domain.MonsterType][]*MonsterTemplate
}

// NewMonsterRegistry creates registry with all monsters from C data.
func NewMonsterRegistry() *MonsterRegistry {
	r := &MonsterRegistry{
		monsters: make(map[string]*MonsterTemplate),
		byType:   make(map[domain.MonsterType][]*MonsterTemplate),
	}
	r.loadMonsters()
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

	for _, m := range monsters {
		r.monsters[m.ID] = m
		r.byType[m.Type] = append(r.byType[m.Type], m)
	}
}

// GetByID returns monster by ID.
func (r *MonsterRegistry) GetByID(id string) *MonsterTemplate {
	return r.monsters[id]
}

// GetByType returns all monsters of given type.
func (r *MonsterRegistry) GetByType(t domain.MonsterType) []*MonsterTemplate {
	return r.byType[t]
}

// GetRandom returns a random monster of given type.
func (r *MonsterRegistry) GetRandom(t domain.MonsterType) *MonsterTemplate {
	list := r.byType[t]
	if len(list) == 0 {
		return nil
	}
	return list[rand.Intn(len(list))]
}

// GetAll returns all monsters.
func (r *MonsterRegistry) GetAll() []*MonsterTemplate {
	result := make([]*MonsterTemplate, 0, len(r.monsters))
	for _, m := range r.monsters {
		result = append(result, m)
	}
	return result
}
