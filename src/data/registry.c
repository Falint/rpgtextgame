/**
 * @file registry.c
 * @brief Central registry implementation
 *
 * Provides lookup functions for all game entities.
 * Uses lazy initialization to load databases on first access.
 */

#include "registry.h"
#include "items.h"
#include "monsters.h"
#include "shop_stock.h"
#include "upgrades.h"
#include "weapons.h"
#include <string.h>

/* ============================================
 * CACHED DATABASE POINTERS
 * ============================================ */
static const MonsterTemplate *g_monsters = NULL;
static int g_monster_count = 0;

static const WeaponTemplate *g_weapons = NULL;
static int g_weapon_count = 0;

static const ItemTemplate *g_items = NULL;
static int g_item_count = 0;

static const UpgradeRecipe *g_upgrades = NULL;
static int g_upgrade_count = 0;

static const ShopEntry *g_shop = NULL;
static int g_shop_count = 0;

static int g_initialized = 0;

/* ============================================
 * INITIALIZATION
 * ============================================ */

void registry_init(void) {
  if (g_initialized) {
    return;
  }

  g_monsters = monsters_get_database(&g_monster_count);
  g_weapons = weapons_get_database(&g_weapon_count);
  g_items = items_get_database(&g_item_count);
  g_upgrades = upgrades_get_database(&g_upgrade_count);
  g_shop = shop_stock_get_database(&g_shop_count);

  g_initialized = 1;
}

/* Helper to ensure initialization */
static void ensure_init(void) {
  if (!g_initialized) {
    registry_init();
  }
}

/* ============================================
 * MONSTER REGISTRY
 * ============================================ */

const MonsterTemplate *registry_get_monster(const char *id) {
  ensure_init();
  if (!id)
    return NULL;

  for (int i = 0; i < g_monster_count; i++) {
    if (strcmp(g_monsters[i].id, id) == 0) {
      return &g_monsters[i];
    }
  }
  return NULL;
}

const MonsterTemplate *registry_get_monster_by_index(int index) {
  ensure_init();
  if (index < 0 || index >= g_monster_count) {
    return NULL;
  }
  return &g_monsters[index];
}

int registry_get_monster_count(void) {
  ensure_init();
  return g_monster_count;
}

int registry_get_monsters_by_type(MonsterType type,
                                  const MonsterTemplate **out_array,
                                  int max_count) {
  ensure_init();
  if (!out_array || max_count <= 0) {
    return 0;
  }

  int found = 0;
  for (int i = 0; i < g_monster_count && found < max_count; i++) {
    if (g_monsters[i].type == type) {
      out_array[found++] = &g_monsters[i];
    }
  }
  return found;
}

/* ============================================
 * WEAPON REGISTRY
 * ============================================ */

const WeaponTemplate *registry_get_weapon(const char *id) {
  ensure_init();
  if (!id)
    return NULL;

  for (int i = 0; i < g_weapon_count; i++) {
    if (strcmp(g_weapons[i].id, id) == 0) {
      return &g_weapons[i];
    }
  }
  return NULL;
}

const WeaponTemplate *registry_get_weapon_by_index(int index) {
  ensure_init();
  if (index < 0 || index >= g_weapon_count) {
    return NULL;
  }
  return &g_weapons[index];
}

int registry_get_weapon_count(void) {
  ensure_init();
  return g_weapon_count;
}

/* ============================================
 * ITEM REGISTRY
 * ============================================ */

const ItemTemplate *registry_get_item(const char *id) {
  ensure_init();
  if (!id)
    return NULL;

  for (int i = 0; i < g_item_count; i++) {
    if (strcmp(g_items[i].id, id) == 0) {
      return &g_items[i];
    }
  }
  return NULL;
}

const ItemTemplate *registry_get_item_by_index(int index) {
  ensure_init();
  if (index < 0 || index >= g_item_count) {
    return NULL;
  }
  return &g_items[index];
}

int registry_get_item_count(void) {
  ensure_init();
  return g_item_count;
}

int registry_get_items_by_category(ItemCategory category,
                                   const ItemTemplate **out_array,
                                   int max_count) {
  ensure_init();
  if (!out_array || max_count <= 0) {
    return 0;
  }

  int found = 0;
  for (int i = 0; i < g_item_count && found < max_count; i++) {
    if (g_items[i].category == category) {
      out_array[found++] = &g_items[i];
    }
  }
  return found;
}

/* ============================================
 * UPGRADE REGISTRY
 * ============================================ */

const UpgradeRecipe *registry_get_upgrade(const char *weapon_id,
                                          int from_tier) {
  ensure_init();
  if (!weapon_id)
    return NULL;

  for (int i = 0; i < g_upgrade_count; i++) {
    if (strcmp(g_upgrades[i].weapon_id, weapon_id) == 0 &&
        g_upgrades[i].from_tier == from_tier) {
      return &g_upgrades[i];
    }
  }
  return NULL;
}

int registry_get_upgrade_count(void) {
  ensure_init();
  return g_upgrade_count;
}

/* ============================================
 * SHOP REGISTRY
 * ============================================ */

const ShopEntry *registry_get_shop_entries(int *out_count) {
  ensure_init();
  if (out_count) {
    *out_count = g_shop_count;
  }
  return g_shop;
}
