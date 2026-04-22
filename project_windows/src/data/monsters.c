/**
 * @file monsters.c
 * @brief Monster Database
 *
 * ============================================
 * HOW TO ADD A NEW MONSTER:
 * ============================================
 * 1. Add a new entry to MONSTER_DB array below
 * 2. Fill in all required fields
 * 3. That's it! No other code changes needed.
 *
 * The monster will automatically be available for spawning.
 */

#include "monsters.h"

static const MonsterTemplate MONSTER_DB[] = {
    /* =========================================
     * NORMAL MONSTERS - Common encounters
     * ========================================= */
    {.id = "slime",
     .name = "Slime",
     .base_hp = 30,
     .base_attack = 8,
     .base_defense = 2,
     .element = ELEMENT_WATER,
     .type = MONSTER_TYPE_NORMAL,
     .gold_min = 10,
     .gold_max = 20,
     .xp_reward = 15,
     .loot = {{"slime_gel", 0.5f}, {"small_potion", 0.15f}},
     .loot_count = 2},
    {.id = "goblin",
     .name = "Goblin",
     .base_hp = 40,
     .base_attack = 12,
     .base_defense = 5,
     .element = ELEMENT_EARTH,
     .type = MONSTER_TYPE_NORMAL,
     .gold_min = 15,
     .gold_max = 30,
     .xp_reward = 20,
     .loot = {{"goblin_fang", 0.4f}, {"small_potion", 0.1f}},
     .loot_count = 2},
    {.id = "bat",
     .name = "Cave Bat",
     .base_hp = 25,
     .base_attack = 10,
     .base_defense = 3,
     .element = ELEMENT_WIND,
     .type = MONSTER_TYPE_NORMAL,
     .gold_min = 8,
     .gold_max = 18,
     .xp_reward = 12,
     .loot = {{"bat_wing", 0.45f}},
     .loot_count = 1},
    {.id = "wolf",
     .name = "Wild Wolf",
     .base_hp = 45,
     .base_attack = 14,
     .base_defense = 4,
     .element = ELEMENT_NONE,
     .type = MONSTER_TYPE_NORMAL,
     .gold_min = 12,
     .gold_max = 25,
     .xp_reward = 22,
     .loot = {{"wolf_pelt", 0.5f}, {"wolf_fang", 0.3f}},
     .loot_count = 2},
    {.id = "skeleton",
     .name = "Skeleton",
     .base_hp = 35,
     .base_attack = 11,
     .base_defense = 6,
     .element = ELEMENT_NONE,
     .type = MONSTER_TYPE_NORMAL,
     .gold_min = 18,
     .gold_max = 35,
     .xp_reward = 25,
     .loot = {{"bone_fragment", 0.6f}, {"rusty_sword", 0.05f}},
     .loot_count = 2},

    /* =========================================
     * ELITE MONSTERS - Stronger, better drops
     * ========================================= */
    {.id = "fire_spirit",
     .name = "Fire Spirit",
     .base_hp = 80,
     .base_attack = 22,
     .base_defense = 8,
     .element = ELEMENT_FIRE,
     .type = MONSTER_TYPE_ELITE,
     .gold_min = 50,
     .gold_max = 100,
     .xp_reward = 60,
     .loot = {{"fire_essence", 0.6f},
              {"ember_stone", 0.3f},
              {"medium_potion", 0.2f}},
     .loot_count = 3},
    {.id = "water_elemental",
     .name = "Water Elemental",
     .base_hp = 90,
     .base_attack = 20,
     .base_defense = 12,
     .element = ELEMENT_WATER,
     .type = MONSTER_TYPE_ELITE,
     .gold_min = 55,
     .gold_max = 110,
     .xp_reward = 65,
     .loot = {{"water_essence", 0.6f},
              {"aqua_crystal", 0.25f},
              {"medium_potion", 0.2f}},
     .loot_count = 3},
    {.id = "stone_golem",
     .name = "Stone Golem",
     .base_hp = 120,
     .base_attack = 18,
     .base_defense = 20,
     .element = ELEMENT_EARTH,
     .type = MONSTER_TYPE_ELITE,
     .gold_min = 60,
     .gold_max = 120,
     .xp_reward = 70,
     .loot = {{"earth_essence", 0.6f}, {"stone_core", 0.35f}},
     .loot_count = 2},
    {.id = "wind_wraith",
     .name = "Wind Wraith",
     .base_hp = 70,
     .base_attack = 25,
     .base_defense = 5,
     .element = ELEMENT_WIND,
     .type = MONSTER_TYPE_ELITE,
     .gold_min = 45,
     .gold_max = 95,
     .xp_reward = 55,
     .loot = {{"wind_essence", 0.6f}, {"gale_feather", 0.4f}},
     .loot_count = 2},
    {.id = "orc_warrior",
     .name = "Orc Warrior",
     .base_hp = 100,
     .base_attack = 24,
     .base_defense = 10,
     .element = ELEMENT_NONE,
     .type = MONSTER_TYPE_ELITE,
     .gold_min = 70,
     .gold_max = 140,
     .xp_reward = 75,
     .loot = {{"orc_tusk", 0.5f},
              {"battle_axe", 0.1f},
              {"large_potion", 0.15f}},
     .loot_count = 3},

    /* =========================================
     * BOSS MONSTERS - Very strong, guaranteed special drops
     * ========================================= */
    {.id = "ancient_dragon",
     .name = "Ancient Dragon",
     .base_hp = 500,
     .base_attack = 50,
     .base_defense = 30,
     .element = ELEMENT_FIRE,
     .type = MONSTER_TYPE_BOSS,
     .gold_min = 500,
     .gold_max = 1000,
     .xp_reward = 500,
     .loot = {{"dragon_scale", 1.0f}, /* Guaranteed */
              {"dragon_fang", 0.6f},
              {"legendary_ore", 0.3f},
              {"large_potion", 0.8f}},
     .loot_count = 4},
    {.id = "kraken",
     .name = "Kraken",
     .base_hp = 450,
     .base_attack = 45,
     .base_defense = 25,
     .element = ELEMENT_WATER,
     .type = MONSTER_TYPE_BOSS,
     .gold_min = 450,
     .gold_max = 900,
     .xp_reward = 450,
     .loot = {{"kraken_tentacle", 1.0f}, /* Guaranteed */
              {"deep_sea_pearl", 0.5f},
              {"legendary_ore", 0.25f}},
     .loot_count = 3},
    {.id = "earth_titan",
     .name = "Earth Titan",
     .base_hp = 600,
     .base_attack = 40,
     .base_defense = 40,
     .element = ELEMENT_EARTH,
     .type = MONSTER_TYPE_BOSS,
     .gold_min = 550,
     .gold_max = 1100,
     .xp_reward = 550,
     .loot = {{"titan_heart", 1.0f}, /* Guaranteed */
              {"adamant_ore", 0.7f},
              {"legendary_ore", 0.35f}},
     .loot_count = 3}};

#define MONSTER_DB_COUNT (int)(sizeof(MONSTER_DB) / sizeof(MONSTER_DB[0]))

const MonsterTemplate *monsters_get_database(int *out_count) {
  if (out_count) {
    *out_count = MONSTER_DB_COUNT;
  }
  return MONSTER_DB;
}
