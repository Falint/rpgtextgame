/**
 * @file shop_stock.c
 * @brief Shop Inventory Database
 *
 * ============================================
 * HOW TO ADD ITEMS TO SHOP:
 * ============================================
 * 1. Add entry with item_id from items.c or weapons.c
 * 2. Set is_weapon = 1 for weapons, 0 for items
 * 3. Set stock = -1 for unlimited, or a number for limited
 */

#include "shop_stock.h"

static const ShopEntry SHOP_DB[] = {
    /* =========================================
     * WEAPONS FOR SALE
     * ========================================= */
    {.item_id = "iron_sword", .is_weapon = 1, .stock = -1},
    {.item_id = "steel_sword", .is_weapon = 1, .stock = -1},
    {.item_id = "fire_blade", .is_weapon = 1, .stock = 1},
    {.item_id = "aqua_saber", .is_weapon = 1, .stock = 1},
    {.item_id = "earth_cleaver", .is_weapon = 1, .stock = 1},
    {.item_id = "wind_cutter", .is_weapon = 1, .stock = 1},
    {.item_id = "iron_axe", .is_weapon = 1, .stock = -1},
    {.item_id = "wooden_staff", .is_weapon = 1, .stock = -1},
    {.item_id = "fire_staff", .is_weapon = 1, .stock = 1},
    {.item_id = "ice_staff", .is_weapon = 1, .stock = 1},
    {.item_id = "iron_dagger", .is_weapon = 1, .stock = -1},
    {.item_id = "assassin_blade", .is_weapon = 1, .stock = 1},
    {.item_id = "hunting_bow", .is_weapon = 1, .stock = -1},
    {.item_id = "gale_bow", .is_weapon = 1, .stock = 1},

    /* =========================================
     * CONSUMABLES FOR SALE
     * ========================================= */
    {.item_id = "small_potion", .is_weapon = 0, .stock = -1},
    {.item_id = "medium_potion", .is_weapon = 0, .stock = -1},
    {.item_id = "large_potion", .is_weapon = 0, .stock = -1},
    {.item_id = "full_restore", .is_weapon = 0, .stock = 5},
    {.item_id = "atk_elixir", .is_weapon = 0, .stock = -1},
    {.item_id = "def_elixir", .is_weapon = 0, .stock = -1},
    {.item_id = "power_surge", .is_weapon = 0, .stock = 3},
    {.item_id = "iron_skin", .is_weapon = 0, .stock = 3},
    {.item_id = "antidote", .is_weapon = 0, .stock = -1}};

#define SHOP_DB_COUNT (int)(sizeof(SHOP_DB) / sizeof(SHOP_DB[0]))

const ShopEntry *shop_stock_get_database(int *out_count) {
  if (out_count) {
    *out_count = SHOP_DB_COUNT;
  }
  return SHOP_DB;
}
