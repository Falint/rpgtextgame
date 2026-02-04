/**
 * @file weapons.c
 * @brief Weapon Database
 *
 * ============================================
 * HOW TO ADD A NEW WEAPON:
 * ============================================
 * 1. Add a new entry to WEAPON_DB array below
 * 2. Fill in all required fields
 * 3. If upgradeable, add recipes in upgrades.c
 * 4. If purchasable, add to shop in shop_stock.c
 */

#include "weapons.h"

static const WeaponTemplate WEAPON_DB[] = {
    /* =========================================
     * STARTER / BASIC WEAPONS
     * ========================================= */
    {.id = "fists",
     .name = "Fists",
     .description = "Your bare hands. Better find a real weapon.",
     .type = WEAPON_TYPE_FIST,
     .element = ELEMENT_NONE,
     .base_damage = 5,
     .buy_price = 0,
     .sell_price = 0,
     .max_tier = 1},
    {.id = "rusty_sword",
     .name = "Rusty Sword",
     .description = "An old sword, covered in rust but still usable.",
     .type = WEAPON_TYPE_SWORD,
     .element = ELEMENT_NONE,
     .base_damage = 8,
     .buy_price = 0, /* Found only */
     .sell_price = 15,
     .max_tier = 1},

    /* =========================================
     * SWORDS - Balanced weapons
     * ========================================= */
    {.id = "iron_sword",
     .name = "Iron Sword",
     .description = "A sturdy iron blade. Reliable and upgradeable.",
     .type = WEAPON_TYPE_SWORD,
     .element = ELEMENT_NONE,
     .base_damage = 15,
     .buy_price = 100,
     .sell_price = 50,
     .max_tier = 5},
    {.id = "steel_sword",
     .name = "Steel Sword",
     .description = "Forged from high-quality steel. Sharp and durable.",
     .type = WEAPON_TYPE_SWORD,
     .element = ELEMENT_NONE,
     .base_damage = 22,
     .buy_price = 250,
     .sell_price = 125,
     .max_tier = 5},
    {.id = "fire_blade",
     .name = "Fire Blade",
     .description = "A sword imbued with flames. Effective against wind.",
     .type = WEAPON_TYPE_SWORD,
     .element = ELEMENT_FIRE,
     .base_damage = 20,
     .buy_price = 300,
     .sell_price = 150,
     .max_tier = 5},
    {.id = "aqua_saber",
     .name = "Aqua Saber",
     .description = "A blade of crystallized water. Strong against fire.",
     .type = WEAPON_TYPE_SWORD,
     .element = ELEMENT_WATER,
     .base_damage = 20,
     .buy_price = 300,
     .sell_price = 150,
     .max_tier = 5},
    {.id = "earth_cleaver",
     .name = "Earth Cleaver",
     .description = "Heavy blade infused with earth magic.",
     .type = WEAPON_TYPE_SWORD,
     .element = ELEMENT_EARTH,
     .base_damage = 20,
     .buy_price = 300,
     .sell_price = 150,
     .max_tier = 5},
    {.id = "wind_cutter",
     .name = "Wind Cutter",
     .description = "Swift blade that cuts like the wind.",
     .type = WEAPON_TYPE_SWORD,
     .element = ELEMENT_WIND,
     .base_damage = 18,
     .buy_price = 280,
     .sell_price = 140,
     .max_tier = 5},

    /* =========================================
     * AXES - High damage, slower
     * ========================================= */
    {.id = "iron_axe",
     .name = "Iron Axe",
     .description = "A heavy axe. Deals massive damage.",
     .type = WEAPON_TYPE_AXE,
     .element = ELEMENT_NONE,
     .base_damage = 20,
     .buy_price = 150,
     .sell_price = 75,
     .max_tier = 5},
    {.id = "battle_axe",
     .name = "Battle Axe",
     .description = "A warrior's axe, forged for combat.",
     .type = WEAPON_TYPE_AXE,
     .element = ELEMENT_NONE,
     .base_damage = 28,
     .buy_price = 0, /* Drop only */
     .sell_price = 100,
     .max_tier = 5},

    /* =========================================
     * STAVES - Magic-focused
     * ========================================= */
    {.id = "wooden_staff",
     .name = "Wooden Staff",
     .description = "A simple staff. Good for beginners.",
     .type = WEAPON_TYPE_STAFF,
     .element = ELEMENT_NONE,
     .base_damage = 10,
     .buy_price = 80,
     .sell_price = 40,
     .max_tier = 3},
    {.id = "fire_staff",
     .name = "Fire Staff",
     .description = "Channels the power of flames.",
     .type = WEAPON_TYPE_STAFF,
     .element = ELEMENT_FIRE,
     .base_damage = 18,
     .buy_price = 280,
     .sell_price = 140,
     .max_tier = 5},
    {.id = "ice_staff",
     .name = "Ice Staff",
     .description = "Frozen magic flows through this staff.",
     .type = WEAPON_TYPE_STAFF,
     .element = ELEMENT_WATER,
     .base_damage = 18,
     .buy_price = 280,
     .sell_price = 140,
     .max_tier = 5},

    /* =========================================
     * DAGGERS - Fast, lower damage
     * ========================================= */
    {.id = "iron_dagger",
     .name = "Iron Dagger",
     .description = "Quick and deadly. Perfect for swift strikes.",
     .type = WEAPON_TYPE_DAGGER,
     .element = ELEMENT_NONE,
     .base_damage = 12,
     .buy_price = 80,
     .sell_price = 40,
     .max_tier = 5},
    {.id = "assassin_blade",
     .name = "Assassin Blade",
     .description = "A shadowy dagger. Whispers of death.",
     .type = WEAPON_TYPE_DAGGER,
     .element = ELEMENT_WIND,
     .base_damage = 16,
     .buy_price = 220,
     .sell_price = 110,
     .max_tier = 5},

    /* =========================================
     * BOWS - Ranged weapons
     * ========================================= */
    {.id = "hunting_bow",
     .name = "Hunting Bow",
     .description = "A reliable bow for hunting and combat.",
     .type = WEAPON_TYPE_BOW,
     .element = ELEMENT_NONE,
     .base_damage = 14,
     .buy_price = 120,
     .sell_price = 60,
     .max_tier = 5},
    {.id = "gale_bow",
     .name = "Gale Bow",
     .description = "Arrows fly swift as the wind itself.",
     .type = WEAPON_TYPE_BOW,
     .element = ELEMENT_WIND,
     .base_damage = 18,
     .buy_price = 260,
     .sell_price = 130,
     .max_tier = 5},

    /* =========================================
     * LEGENDARY WEAPONS
     * ========================================= */
    {.id = "dragon_slayer",
     .name = "Dragon Slayer",
     .description = "Legendary blade forged from dragon scales.",
     .type = WEAPON_TYPE_SWORD,
     .element = ELEMENT_FIRE,
     .base_damage = 45,
     .buy_price = 0, /* Craft only */
     .sell_price = 500,
     .max_tier = 5},
    {.id = "titan_hammer",
     .name = "Titan's Hammer",
     .description = "Weapon of the earth titans. Immense power.",
     .type = WEAPON_TYPE_AXE,
     .element = ELEMENT_EARTH,
     .base_damage = 50,
     .buy_price = 0, /* Craft only */
     .sell_price = 600,
     .max_tier = 5}};

#define WEAPON_DB_COUNT (int)(sizeof(WEAPON_DB) / sizeof(WEAPON_DB[0]))

const WeaponTemplate *weapons_get_database(int *out_count) {
  if (out_count) {
    *out_count = WEAPON_DB_COUNT;
  }
  return WEAPON_DB;
}
