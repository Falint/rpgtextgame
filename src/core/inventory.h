/**
 * @file inventory.h
 * @brief Inventory system for managing items and weapons
 */

#ifndef INVENTORY_H
#define INVENTORY_H

#include "../types.h"
#include "item.h"
#include "weapon.h"

/**
 * @brief Single inventory slot
 */
struct InventorySlot {
  SlotType type;
  union {
    Weapon weapon;
    Item item;
  } data;
  int quantity; /* For stackable items; weapons always have quantity 1 */
};

/**
 * @brief Inventory container
 */
struct Inventory {
  InventorySlot slots[INVENTORY_MAX_SLOTS];
  int used_slots; /* Number of non-empty slots */
};

/**
 * @brief Initialize empty inventory
 */
void inventory_init(Inventory *inv);

/**
 * @brief Add weapon to inventory
 */
ResultCode inventory_add_weapon(Inventory *inv, Weapon weapon);

/**
 * @brief Add item to inventory (auto-stacks if possible)
 */
ResultCode inventory_add_item(Inventory *inv, Item item, int quantity);

/**
 * @brief Remove entire slot
 */
void inventory_remove_slot(Inventory *inv, int index);

/**
 * @brief Use one item from slot (decrements quantity)
 * @return 1 on success, 0 on failure
 */
int inventory_use_item_at(Inventory *inv, int index);

/**
 * @brief Get slot at index
 * @return Pointer to slot, or NULL if invalid/empty
 */
InventorySlot *inventory_get_slot(Inventory *inv, int index);

/**
 * @brief Count how many of a specific item ID are in inventory
 */
int inventory_count_item(const Inventory *inv, const char *item_id);

/**
 * @brief Remove specific quantity of item by ID
 */
ResultCode inventory_remove_item_by_id(Inventory *inv, const char *item_id,
                                       int quantity);

/**
 * @brief Find slot index containing item with given ID
 * @return Slot index, or -1 if not found
 */
int inventory_find_item(const Inventory *inv, const char *item_id);

/**
 * @brief Check if inventory is full
 */
int inventory_is_full(const Inventory *inv);

/**
 * @brief Get number of free slots
 */
int inventory_free_slots(const Inventory *inv);

/**
 * @brief Print full inventory
 */
void inventory_print(const Inventory *inv);

/**
 * @brief Print only weapons in inventory
 */
void inventory_print_weapons(const Inventory *inv);

/**
 * @brief Print only consumable items
 */
void inventory_print_consumables(const Inventory *inv);

/**
 * @brief Print only materials
 */
void inventory_print_materials(const Inventory *inv);

#endif /* INVENTORY_H */
