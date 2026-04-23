/**
 * @file types.hpp
 * @brief Shared type definitions for TextRPG (C++ Version)
 *
 * File ini berisi semua enum class dan forward declarations.
 * Menggunakan enum class memberikan type-safety (mencegah tabrakan nama).
 */

#ifndef TYPES_HPP
#define TYPES_HPP

#include "config.hpp"
#include <cstdint>

/* ============================================
 * FORWARD DECLARATIONS
 * ============================================ */
// Dalam C++, kita bisa menggunakan 'struct' atau 'class'
struct Player;
struct Enemy;
struct Weapon;
struct Item;
struct Inventory;
struct InventorySlot;
struct Buff;
struct LootDrop;
struct ShopEntry; // Tambahan untuk sistem shop
struct UpgradeRecipe; // Tambahan untuk sistem upgrade

/* ============================================
 * ELEMENT SYSTEM
 * ============================================ */
enum class Element : int32_t {
    NONE = 0,
    FIRE,   /* Kuat vs Wind, Lemah vs Water */
    WATER,  /* Kuat vs Fire, Lemah vs Earth */
    EARTH,  /* Kuat vs Water, Lemah vs Wind */
    WIND,   /* Kuat vs Earth, Lemah vs Fire */
    LIGHT,  /* Kuat vs Dark */
    DARK,   /* Kuat vs Light */
    COUNT
};

/* ============================================
 * ITEM & INVENTORY SYSTEM
 * ============================================ */

enum class ItemCategory : int32_t {
    CONSUMABLE,
    MATERIAL,
    KEY,
    COUNT
};

enum class ConsumableType : int32_t {
    NONE = 0,
    HEAL,
    BUFF_ATK,
    BUFF_DEF,
    CURE_POISON,
    FULL_RESTORE,
    COUNT
};

enum class SlotType : int32_t {
    EMPTY = 0,
    WEAPON,
    ITEM
};

/* ============================================
 * WEAPON & MONSTER SYSTEM
 * ============================================ */

enum class WeaponType : int32_t {
    FIST, SWORD, AXE, STAFF, BOW, DAGGER, COUNT
};

enum class MonsterType : int32_t {
    NORMAL, ELITE, BOSS, COUNT
};

/* ============================================
 * STATUS & RESULT CODES
 * ============================================ */

enum class ResultCode : int32_t {
    SUCCESS = 0,
    FAIL,
    INVENTORY_FULL,
    NOT_ENOUGH_GOLD,
    NOT_ENOUGH_MATERIALS,
    ITEM_NOT_FOUND,
    INVALID_SLOT,
    INVALID_TARGET,
    MAX_TIER_REACHED,
    CANNOT_UPGRADE
};

enum class MenuResult : int32_t {
    CONTINUE = 0,
    EXIT,
    BACK
};

enum class BattleResult : int32_t {
    WIN = 0,
    LOSE,
    ESCAPE
};

#endif // TYPES_HPP