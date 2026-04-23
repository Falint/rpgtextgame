/**
 * @file item.h
 * @brief Item system with consumables and materials
 */

#ifndef ITEM_H
#define ITEM_H

#include "../data/registry.h"
#include "../types.h"

/* Forward declaration */
struct Player;

/**
 * @brief Runtime item instance
 */
struct Item {
  char id[MAX_ID_LEN];
  char name[MAX_NAME_LEN];
  char description[MAX_DESC_LEN];
  ItemCategory category;
  ConsumableType consumable_type;
  int value;
  int duration;
  int sell_price;
};

/**
 * @brief Create item from template ID
 */
Item item_create(const char *template_id);

/**
 * @brief Create item from template
 */
Item item_create_from_template(const ItemTemplate *tmpl);

/**
 * @brief Check if item is consumable
 */
int item_is_consumable(const Item *item);

/**
 * @brief Check if item is a material
 */
int item_is_material(const Item *item);

/**
 * @brief Get consumable type as string
 */
const char *consumable_type_to_string(ConsumableType type);

/**
 * @brief Get item category as string
 */
const char *item_category_to_string(ItemCategory cat);

/**
 * @brief Print item info (single line)
 */
void item_print(const Item *item);

/**
 * @brief Print detailed item info
 */
void item_print_detailed(const Item *item);

/**
 * @brief Check if two items are the same (for stacking)
 */
int item_equals(const Item *a, const Item *b);

#endif /* ITEM_H */
