/**
 * @file registry.cpp
 * @brief Central registry implementation (C++ Version)
 */

#include "registry.hpp"
#include "items.hpp"
#include "monsters.hpp"
#include "weapons.hpp"
#include <unordered_map>
#include <algorithm>

namespace Registry {

/* ============================================
 * INTERNAL STORAGE (Hidden from other files)
 * ============================================ */

static bool g_initialized = false;

// Maps untuk pencarian instan berdasarkan ID string
static std::unordered_map<std::string, const MonsterTemplate*> monster_map;
static std::unordered_map<std::string, const WeaponTemplate*> weapon_map;
static std::unordered_map<std::string, const ItemTemplate*> item_map;

// Penyimpanan database mentah (untuk akses via index)
static const MonsterTemplate* g_monsters = nullptr;
static int32_t g_monster_count = 0;

static const WeaponTemplate* g_weapons = nullptr;
static int32_t g_weapon_count = 0;

static const ItemTemplate* g_items = nullptr;
static int32_t g_item_count = 0;

/* ============================================
 * INITIALIZATION
 * ============================================ */

void init() {
    if (g_initialized) return;

    // 1. Muat Database Monster
    g_monsters = MonsterDatabase::get_database(&g_monster_count);
    for (int i = 0; i < g_monster_count; ++i) {
        monster_map[g_monsters[i].id] = &g_monsters[i];
    }

    // 2. Muat Database Senjata
    g_weapons = WeaponDatabase::get_database(&g_weapon_count);
    for (int i = 0; i < g_weapon_count; ++i) {
        weapon_map[g_weapons[i].id] = &g_weapons[i];
    }

    // 3. Muat Database Item
    g_items = ItemDatabase::get_database(&g_item_count);
    for (int i = 0; i < g_item_count; ++i) {
        item_map[g_items[i].id] = &g_items[i];
    }

    g_initialized = true;
}

static void ensure_init() {
    if (!g_initialized) init();
}

/* ============================================
 * MONSTER LOOKUPS
 * ============================================ */

const MonsterTemplate* get_monster(const std::string& id) {
    ensure_init();
    auto it = monster_map.find(id);
    return (it != monster_map.end()) ? it->second : nullptr;
}

size_t get_monster_count() {
    ensure_init();
    return static_cast<size_t>(g_monster_count);
}

std::vector<const MonsterTemplate*> get_monsters_by_type(MonsterType type) {
    ensure_init();
    std::vector<const MonsterTemplate*> results;
    
    for (int i = 0; i < g_monster_count; ++i) {
        if (g_monsters[i].type == type) {
            results.push_back(&g_monsters[i]);
        }
    }
    return results;
}

/* ============================================
 * WEAPON LOOKUPS
 * ============================================ */

const WeaponTemplate* get_weapon(const std::string& id) {
    ensure_init();
    auto it = weapon_map.find(id);
    return (it != weapon_map.end()) ? it->second : nullptr;
}

size_t get_weapon_count() {
    ensure_init();
    return static_cast<size_t>(g_weapon_count);
}

/* ============================================
 * ITEM LOOKUPS
 * ============================================ */

const ItemTemplate* get_item(const std::string& id) {
    ensure_init();
    auto it = item_map.find(id);
    return (it != item_map.end()) ? it->second : nullptr;
}

size_t get_item_count() {
    ensure_init();
    return static_cast<size_t>(g_item_count);
}

std::vector<const ItemTemplate*> get_items_by_category(ItemCategory category) {
    ensure_init();
    std::vector<const ItemTemplate*> results;

    for (int i = 0; i < g_item_count; ++i) {
        if (g_items[i].category == category) {
            results.push_back(&g_items[i]);
        }
    }
    return results;
}

} // namespace Registry