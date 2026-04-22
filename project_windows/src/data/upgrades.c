/**
 * @file upgrades.c
 * @brief Weapon Upgrade Recipes Database
 *
 * ============================================
 * HOW TO ADD A NEW UPGRADE PATH:
 * ============================================
 * 1. Add entries for each tier transition
 * 2. Specify required materials and quantities
 * 3. Set gold cost and damage bonus
 * 4. Ensure materials exist in items.c
 */

#include "upgrades.h"

static const UpgradeRecipe UPGRADE_DB[] = {
    /* =========================================
     * IRON SWORD UPGRADES
     * ========================================= */
    {.weapon_id = "iron_sword",
     .from_tier = 1,
     .to_tier = 2,
     .gold_cost = 80,
     .materials = {{"goblin_fang", 3}, {"slime_gel", 2}},
     .material_count = 2,
     .damage_bonus = 3},
    {.weapon_id = "iron_sword",
     .from_tier = 2,
     .to_tier = 3,
     .gold_cost = 150,
     .materials = {{"goblin_fang", 5}, {"wolf_fang", 3}},
     .material_count = 2,
     .damage_bonus = 4},
    {.weapon_id = "iron_sword",
     .from_tier = 3,
     .to_tier = 4,
     .gold_cost = 300,
     .materials = {{"orc_tusk", 3}, {"bone_fragment", 5}},
     .material_count = 2,
     .damage_bonus = 5},
    {.weapon_id = "iron_sword",
     .from_tier = 4,
     .to_tier = 5,
     .gold_cost = 500,
     .materials = {{"legendary_ore", 1}, {"orc_tusk", 5}},
     .material_count = 2,
     .damage_bonus = 8},

    /* =========================================
     * FIRE BLADE UPGRADES
     * ========================================= */
    {.weapon_id = "fire_blade",
     .from_tier = 1,
     .to_tier = 2,
     .gold_cost = 120,
     .materials = {{"fire_essence", 2}, {"ember_stone", 1}},
     .material_count = 2,
     .damage_bonus = 4},
    {.weapon_id = "fire_blade",
     .from_tier = 2,
     .to_tier = 3,
     .gold_cost = 250,
     .materials = {{"fire_essence", 4}, {"ember_stone", 3}},
     .material_count = 2,
     .damage_bonus = 5},
    {.weapon_id = "fire_blade",
     .from_tier = 3,
     .to_tier = 4,
     .gold_cost = 450,
     .materials = {{"fire_essence", 6}, {"dragon_fang", 1}},
     .material_count = 2,
     .damage_bonus = 7},
    {.weapon_id = "fire_blade",
     .from_tier = 4,
     .to_tier = 5,
     .gold_cost = 800,
     .materials = {{"dragon_scale", 2}, {"legendary_ore", 1}},
     .material_count = 2,
     .damage_bonus = 10},

    /* =========================================
     * AQUA SABER UPGRADES
     * ========================================= */
    {.weapon_id = "aqua_saber",
     .from_tier = 1,
     .to_tier = 2,
     .gold_cost = 120,
     .materials = {{"water_essence", 2}, {"aqua_crystal", 1}},
     .material_count = 2,
     .damage_bonus = 4},
    {.weapon_id = "aqua_saber",
     .from_tier = 2,
     .to_tier = 3,
     .gold_cost = 250,
     .materials = {{"water_essence", 4}, {"aqua_crystal", 3}},
     .material_count = 2,
     .damage_bonus = 5},
    {.weapon_id = "aqua_saber",
     .from_tier = 3,
     .to_tier = 4,
     .gold_cost = 450,
     .materials = {{"water_essence", 6}, {"deep_sea_pearl", 1}},
     .material_count = 2,
     .damage_bonus = 7},
    {.weapon_id = "aqua_saber",
     .from_tier = 4,
     .to_tier = 5,
     .gold_cost = 800,
     .materials = {{"kraken_tentacle", 2}, {"legendary_ore", 1}},
     .material_count = 2,
     .damage_bonus = 10},

    /* =========================================
     * EARTH CLEAVER UPGRADES
     * ========================================= */
    {.weapon_id = "earth_cleaver",
     .from_tier = 1,
     .to_tier = 2,
     .gold_cost = 120,
     .materials = {{"earth_essence", 2}, {"stone_core", 1}},
     .material_count = 2,
     .damage_bonus = 4},
    {.weapon_id = "earth_cleaver",
     .from_tier = 2,
     .to_tier = 3,
     .gold_cost = 250,
     .materials = {{"earth_essence", 4}, {"stone_core", 2}},
     .material_count = 2,
     .damage_bonus = 5},
    {.weapon_id = "earth_cleaver",
     .from_tier = 3,
     .to_tier = 4,
     .gold_cost = 450,
     .materials = {{"earth_essence", 6}, {"adamant_ore", 2}},
     .material_count = 2,
     .damage_bonus = 7},
    {.weapon_id = "earth_cleaver",
     .from_tier = 4,
     .to_tier = 5,
     .gold_cost = 800,
     .materials = {{"titan_heart", 1}, {"legendary_ore", 1}},
     .material_count = 2,
     .damage_bonus = 10},

    /* =========================================
     * WIND CUTTER UPGRADES
     * ========================================= */
    {.weapon_id = "wind_cutter",
     .from_tier = 1,
     .to_tier = 2,
     .gold_cost = 100,
     .materials = {{"wind_essence", 2}, {"gale_feather", 2}},
     .material_count = 2,
     .damage_bonus = 3},
    {.weapon_id = "wind_cutter",
     .from_tier = 2,
     .to_tier = 3,
     .gold_cost = 220,
     .materials = {{"wind_essence", 4}, {"gale_feather", 4}},
     .material_count = 2,
     .damage_bonus = 4},
    {.weapon_id = "wind_cutter",
     .from_tier = 3,
     .to_tier = 4,
     .gold_cost = 400,
     .materials = {{"wind_essence", 6}, {"bat_wing", 10}},
     .material_count = 2,
     .damage_bonus = 6},
    {.weapon_id = "wind_cutter",
     .from_tier = 4,
     .to_tier = 5,
     .gold_cost = 700,
     .materials = {{"gale_feather", 10}, {"legendary_ore", 1}},
     .material_count = 2,
     .damage_bonus = 9},

    /* =========================================
     * IRON AXE UPGRADES
     * ========================================= */
    {.weapon_id = "iron_axe",
     .from_tier = 1,
     .to_tier = 2,
     .gold_cost = 100,
     .materials = {{"wolf_pelt", 3}, {"bone_fragment", 3}},
     .material_count = 2,
     .damage_bonus = 4},
    {.weapon_id = "iron_axe",
     .from_tier = 2,
     .to_tier = 3,
     .gold_cost = 200,
     .materials = {{"orc_tusk", 2}, {"wolf_fang", 5}},
     .material_count = 2,
     .damage_bonus = 5},
    {.weapon_id = "iron_axe",
     .from_tier = 3,
     .to_tier = 4,
     .gold_cost = 400,
     .materials = {{"orc_tusk", 5}, {"stone_core", 2}},
     .material_count = 2,
     .damage_bonus = 7},
    {.weapon_id = "iron_axe",
     .from_tier = 4,
     .to_tier = 5,
     .gold_cost = 700,
     .materials = {{"adamant_ore", 3}, {"legendary_ore", 1}},
     .material_count = 2,
     .damage_bonus = 10}};

#define UPGRADE_DB_COUNT (int)(sizeof(UPGRADE_DB) / sizeof(UPGRADE_DB[0]))

const UpgradeRecipe *upgrades_get_database(int *out_count) {
  if (out_count) {
    *out_count = UPGRADE_DB_COUNT;
  }
  return UPGRADE_DB;
}
