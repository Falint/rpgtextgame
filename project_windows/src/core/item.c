/**
 * @file item.c
 * @brief Item system implementation
 */

#include "item.h"
#include "../utils/utils.h"
#include <stdio.h>
#include <string.h>

Item item_create(const char *template_id) {
  Item item = {0};

  const ItemTemplate *tmpl = registry_get_item(template_id);
  if (!tmpl) {
    /* Return empty item */
    safe_strcpy(item.id, "unknown", MAX_ID_LEN);
    safe_strcpy(item.name, "Unknown Item", MAX_NAME_LEN);
    safe_strcpy(item.description, "???", MAX_DESC_LEN);
    item.category = ITEM_CATEGORY_MATERIAL;
    item.consumable_type = CONSUMABLE_NONE;
    item.value = 0;
    item.duration = 0;
    item.sell_price = 0;
    return item;
  }

  return item_create_from_template(tmpl);
}

Item item_create_from_template(const ItemTemplate *tmpl) {
  Item item = {0};

  if (!tmpl) {
    return item_create("unknown");
  }

  safe_strcpy(item.id, tmpl->id, MAX_ID_LEN);
  safe_strcpy(item.name, tmpl->name, MAX_NAME_LEN);
  safe_strcpy(item.description, tmpl->description, MAX_DESC_LEN);
  item.category = tmpl->category;
  item.consumable_type = tmpl->consumable_type;
  item.value = tmpl->value;
  item.duration = tmpl->duration;
  item.sell_price = tmpl->sell_price;

  return item;
}

int item_is_consumable(const Item *item) {
  if (!item)
    return 0;
  return item->category == ITEM_CATEGORY_CONSUMABLE;
}

int item_is_material(const Item *item) {
  if (!item)
    return 0;
  return item->category == ITEM_CATEGORY_MATERIAL;
}

const char *consumable_type_to_string(ConsumableType type) {
  switch (type) {
  case CONSUMABLE_HEAL:
    return "Healing";
  case CONSUMABLE_BUFF_ATK:
    return "ATK Buff";
  case CONSUMABLE_BUFF_DEF:
    return "DEF Buff";
  case CONSUMABLE_CURE_POISON:
    return "Cure";
  case CONSUMABLE_FULL_RESTORE:
    return "Full Restore";
  default:
    return "Unknown";
  }
}

const char *item_category_to_string(ItemCategory cat) {
  switch (cat) {
  case ITEM_CATEGORY_CONSUMABLE:
    return "Consumable";
  case ITEM_CATEGORY_MATERIAL:
    return "Material";
  case ITEM_CATEGORY_KEY:
    return "Key Item";
  default:
    return "Unknown";
  }
}

void item_print(const Item *item) {
  if (!item)
    return;

  printf("[%s]", item->name);

  if (item->category == ITEM_CATEGORY_CONSUMABLE) {
    switch (item->consumable_type) {
    case CONSUMABLE_HEAL:
      printf(" +%d HP", item->value);
      break;
    case CONSUMABLE_BUFF_ATK:
      printf(" +%d%% ATK (%d turns)", item->value, item->duration);
      break;
    case CONSUMABLE_BUFF_DEF:
      printf(" +%d%% DEF (%d turns)", item->value, item->duration);
      break;
    case CONSUMABLE_FULL_RESTORE:
      printf(" Full HP");
      break;
    default:
      break;
    }
  }
}

void item_print_detailed(const Item *item) {
  if (!item)
    return;

  printf("=== %s ===\n", item->name);
  printf("Type: %s\n", item_category_to_string(item->category));

  if (item->category == ITEM_CATEGORY_CONSUMABLE) {
    printf("Effect: %s\n", consumable_type_to_string(item->consumable_type));
    if (item->value > 0) {
      printf("Value: %d\n", item->value);
    }
    if (item->duration > 0) {
      printf("Duration: %d turns\n", item->duration);
    }
  }

  printf("Sell Price: %d G\n", item->sell_price);
  printf("%s\n", item->description);
}

int item_equals(const Item *a, const Item *b) {
  if (!a || !b)
    return 0;
  return strcmp(a->id, b->id) == 0;
}
