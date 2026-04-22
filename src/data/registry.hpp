/**
 * @file registry.hpp
 * @brief Data registry for all game entities (C++ Version)
 */

#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

#include "../types.hpp"
#include "../config.hpp"

/* ============================================
 * TEMPLATE STRUCTURES (Read-Only)
 * ============================================ */

struct LootEntry {
    std::string item_id;
    float drop_chance;
};

struct MonsterTemplate {
    std::string id;
    std::string name;
    int32_t base_hp;
    int32_t base_attack;
    int32_t base_defense;
    Element element;
    MonsterType type;
    int32_t gold_min;
    int32_t gold_max;
    int32_t xp_reward;
    // Menggunakan std::vector agar fleksibel jumlah drop-nya
    std::vector<LootEntry> loot_table; 
};

struct WeaponTemplate {
    std::string id;
    std::string name;
    std::string description;
    WeaponType type;
    Element element;
    int32_t base_damage;
    int32_t buy_price;
    int32_t sell_price;
    int32_t max_tier;
};

struct ItemTemplate {
    std::string id;
    std::string name;
    std::string description;
    ItemCategory category;
    ConsumableType consumable_type;
    int32_t value;
    int32_t duration;
    int32_t buy_price;
    int32_t sell_price;
    bool stackable;
};

struct UpgradeMaterial {
    std::string item_id;
    int32_t quantity;
};

struct UpgradeRecipe {
    std::string weapon_id;
    int32_t from_tier;
    int32_t to_tier;
    int32_t gold_cost;
    std::vector<UpgradeMaterial> materials;
    int32_t damage_bonus;
};

/* ============================================
 * REGISTRY SYSTEM (Static Manager)
 * ============================================ */

namespace Registry {

    /**
     * @brief Inisialisasi Registry (Membangun Hash Map untuk pencarian cepat)
     */
    void init();

    /* Monster Lookups */
    const MonsterTemplate* get_monster(const std::string& id);
    const std::vector<const MonsterTemplate*> get_monsters_by_type(MonsterType type);
    size_t get_monster_count();

    /* Weapon Lookups */
    const WeaponTemplate* get_weapon(const std::string& id);
    size_t get_weapon_count();

    /* Item Lookups */
    const ItemTemplate* get_item(const std::string& id);
    const std::vector<const ItemTemplate*> get_items_by_category(ItemCategory category);
    size_t get_item_count();

    /* Upgrade Lookups */
    const UpgradeRecipe* get_upgrade(const std::string& weapon_id, int32_t from_tier);

} // namespace Registry

#endif // REGISTRY_HPP