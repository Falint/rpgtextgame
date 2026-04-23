/**
 * @file registry.h
 * @brief Data registry for all game entities
 *
 * Implements the Registry Pattern for data-driven entity management.
 * All monsters, weapons, items, and upgrades are defined as templates
 * and accessed through this central registry.
 *
 * EXTENSIBILITY:
 * To add new content, simply add entries to the respective database
 * arrays (monsters.c, weapons.c, items.c, upgrades.c). No code changes needed!
 */

#ifndef REGISTRY_H
#define REGISTRY_H

#include "../types.h"

/* ============================================
 * TEMPLATE STRUCTURES
 * These are read-only definitions used to create instances
 * ============================================ */

/**
 * @brief Single loot entry for monster drop tables
 */
typedef struct {
  const char *item_id; /* ID of item to drop */
  float drop_chance;   /* 0.0 - 1.0 probability */
} LootEntry;

/**
 * @brief Monster template (read-only definition)
 *
 * Used to create Enemy instances at runtime.
 */
typedef struct {
  const char *id;   /* Unique identifier: "slime", "goblin" */
  const char *name; /* Display name: "Slime", "Goblin Warrior" */
  int base_hp;
  int base_attack;
  int base_defense;
  Element element;
  MonsterType type;
  int gold_min;  /* Minimum gold drop */
  int gold_max;  /* Maximum gold drop */
  int xp_reward; /* Experience points (for future use) */
  LootEntry loot[MAX_LOOT_ENTRIES];
  int loot_count;
} MonsterTemplate;

/**
 * @brief Weapon template (read-only definition)
 */
typedef struct {
  const char *id;          /* Unique identifier: "iron_sword" */
  const char *name;        /* Display name */
  const char *description; /* Flavor text */
  WeaponType type;
  Element element;
  int base_damage;
  int buy_price; /* 0 = not purchasable */
  int sell_price;
  int max_tier; /* Maximum upgrade tier (1 = no upgrades) */
} WeaponTemplate;

/**
 * @brief Item template (read-only definition)
 */
typedef struct {
  const char *id;          /* Unique identifier: "health_potion" */
  const char *name;        /* Display name */
  const char *description; /* Flavor text */
  ItemCategory category;
  ConsumableType consumable_type; /* Only used if category == CONSUMABLE */
  int value;                      /* Effect value (heal amount, buff %, etc) */
  int duration;                   /* Buff duration in turns (0 = instant) */
  int buy_price;                  /* 0 = not purchasable (material/drop only) */
  int sell_price;
  int stackable; /* 1 = stackable, 0 = unique */
} ItemTemplate;

/**
 * @brief Upgrade material requirement
 */
typedef struct {
  const char *item_id; /* Required material ID */
  int quantity;        /* Required amount */
} UpgradeMaterial;

/**
 * @brief Weapon upgrade recipe
 */
typedef struct {
  const char *weapon_id; /* Target weapon */
  int from_tier;         /* Starting tier */
  int to_tier;           /* Resulting tier */
  int gold_cost;         /* Gold required */
  UpgradeMaterial materials[MAX_LOOT_ENTRIES];
  int material_count;
  int damage_bonus; /* Additional damage per upgrade */
} UpgradeRecipe;

/**
 * @brief Shop item entry
 */
typedef struct {
  const char *item_id; /* Can be weapon or item ID */
  int is_weapon;       /* 1 = weapon, 0 = item */
  int stock;           /* -1 = unlimited */
} ShopEntry;

/* ============================================
 * REGISTRY FUNCTIONS
 * ============================================ */

/**
 * @brief Initialize the registry system
 * Must be called before any registry lookups
 */
void registry_init(void);

/* ===== Monster Registry ===== */
const MonsterTemplate *registry_get_monster(const char *id);
const MonsterTemplate *registry_get_monster_by_index(int index);
int registry_get_monster_count(void);

/**
 * @brief Get all monsters of a specific type
 * @param type Monster type filter
 * @param out_array Output array (caller provides)
 * @param max_count Maximum entries to return
 * @return Number of monsters found
 */
int registry_get_monsters_by_type(MonsterType type,
                                  const MonsterTemplate **out_array,
                                  int max_count);

/* ===== Weapon Registry ===== */
const WeaponTemplate *registry_get_weapon(const char *id);
const WeaponTemplate *registry_get_weapon_by_index(int index);
int registry_get_weapon_count(void);

/* ===== Item Registry ===== */
const ItemTemplate *registry_get_item(const char *id);
const ItemTemplate *registry_get_item_by_index(int index);
int registry_get_item_count(void);

/**
 * @brief Get all items of a specific category
 */
int registry_get_items_by_category(ItemCategory category,
                                   const ItemTemplate **out_array,
                                   int max_count);

/* ===== Upgrade Registry ===== */
const UpgradeRecipe *registry_get_upgrade(const char *weapon_id, int from_tier);
int registry_get_upgrade_count(void);

/* ===== Shop Registry ===== */
const ShopEntry *registry_get_shop_entries(int *out_count);

#endif /* REGISTRY_H */
