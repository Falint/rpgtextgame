/**
 * @file types.h
 * @brief Shared type definitions for TextRPG
 *
 * Contains all enums and forward declarations used across modules.
 * This file should be included by most other headers.
 */

#ifndef TYPES_H
#define TYPES_H

#include "config.h"

/* ============================================
 * FORWARD DECLARATIONS
 * ============================================ */
typedef struct Player Player;
typedef struct Enemy Enemy;
typedef struct Weapon Weapon;
typedef struct Item Item;
typedef struct Inventory Inventory;
typedef struct InventorySlot InventorySlot;
typedef struct Buff Buff;
typedef struct LootDrop LootDrop;

/* ============================================
 * ELEMENT SYSTEM
 * Rock-paper-scissors style elemental interactions
 * ============================================ */
typedef enum {
  ELEMENT_NONE = 0,
  ELEMENT_FIRE,  /* Strong vs Wind, Weak vs Water */
  ELEMENT_WATER, /* Strong vs Fire, Weak vs Earth */
  ELEMENT_EARTH, /* Strong vs Water, Weak vs Wind */
  ELEMENT_WIND,  /* Strong vs Earth, Weak vs Fire */
  ELEMENT_COUNT
} Element;

/* ============================================
 * ITEM SYSTEM
 * ============================================ */

/* Main item categories */
typedef enum {
  ITEM_CATEGORY_CONSUMABLE, /* Potions, food - used and removed */
  ITEM_CATEGORY_MATERIAL,   /* Crafting/upgrade materials */
  ITEM_CATEGORY_KEY,        /* Quest items (not implemented yet) */
  ITEM_CATEGORY_COUNT
} ItemCategory;

/* Types of consumable effects */
typedef enum {
  CONSUMABLE_NONE = 0,
  CONSUMABLE_HEAL,         /* Restore HP instantly */
  CONSUMABLE_BUFF_ATK,     /* Temporary attack boost */
  CONSUMABLE_BUFF_DEF,     /* Temporary defense boost */
  CONSUMABLE_CURE_POISON,  /* Remove poison status */
  CONSUMABLE_FULL_RESTORE, /* Full HP + cure all */
  CONSUMABLE_COUNT
} ConsumableType;

/* ============================================
 * WEAPON SYSTEM
 * ============================================ */
typedef enum {
  WEAPON_TYPE_FIST,   /* Unarmed */
  WEAPON_TYPE_SWORD,  /* Balanced */
  WEAPON_TYPE_AXE,    /* High damage, slow */
  WEAPON_TYPE_STAFF,  /* Magic-focused */
  WEAPON_TYPE_BOW,    /* Ranged */
  WEAPON_TYPE_DAGGER, /* Fast, low damage */
  WEAPON_TYPE_COUNT
} WeaponType;

/* ============================================
 * MONSTER SYSTEM
 * ============================================ */
typedef enum {
  MONSTER_TYPE_NORMAL, /* Common enemies */
  MONSTER_TYPE_ELITE,  /* Stronger, better drops */
  MONSTER_TYPE_BOSS,   /* Very strong, guaranteed drops */
  MONSTER_TYPE_COUNT
} MonsterType;

/* ============================================
 * INVENTORY SYSTEM
 * ============================================ */
typedef enum { SLOT_EMPTY = 0, SLOT_WEAPON, SLOT_ITEM } SlotType;

/* ============================================
 * RESULT CODES
 * Consistent error handling across all systems
 * ============================================ */
typedef enum {
  RESULT_SUCCESS = 0,
  RESULT_FAIL,
  RESULT_INVENTORY_FULL,
  RESULT_NOT_ENOUGH_GOLD,
  RESULT_NOT_ENOUGH_MATERIALS,
  RESULT_ITEM_NOT_FOUND,
  RESULT_INVALID_SLOT,
  RESULT_INVALID_TARGET,
  RESULT_MAX_TIER_REACHED,
  RESULT_CANNOT_UPGRADE
} ResultCode;

/* ============================================
 * MENU SYSTEM
 * ============================================ */
typedef enum { MENU_CONTINUE = 0, MENU_EXIT, MENU_BACK } MenuResult;

/* ============================================
 * BATTLE OUTCOME
 * ============================================ */
typedef enum { BATTLE_WIN = 0, BATTLE_LOSE, BATTLE_ESCAPE } BattleResult;

#endif /* TYPES_H */
