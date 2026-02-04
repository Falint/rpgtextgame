/**
 * @file inventory.c
 * @brief Inventory system implementation
 */

#include "inventory.h"
#include <stdio.h>
#include <string.h>

void inventory_init(Inventory *inv) {
  if (!inv)
    return;

  for (int i = 0; i < INVENTORY_MAX_SLOTS; i++) {
    inv->slots[i].type = SLOT_EMPTY;
    inv->slots[i].quantity = 0;
  }
  inv->used_slots = 0;
}

ResultCode inventory_add_weapon(Inventory *inv, Weapon weapon) {
  if (!inv)
    return RESULT_FAIL;

  /* Find first empty slot */
  for (int i = 0; i < INVENTORY_MAX_SLOTS; i++) {
    if (inv->slots[i].type == SLOT_EMPTY) {
      inv->slots[i].type = SLOT_WEAPON;
      inv->slots[i].data.weapon = weapon;
      inv->slots[i].quantity = 1;
      inv->used_slots++;
      return RESULT_SUCCESS;
    }
  }

  return RESULT_INVENTORY_FULL;
}

ResultCode inventory_add_item(Inventory *inv, Item item, int quantity) {
  if (!inv || quantity <= 0)
    return RESULT_FAIL;

  /* Try to stack with existing item first */
  for (int i = 0; i < INVENTORY_MAX_SLOTS; i++) {
    if (inv->slots[i].type == SLOT_ITEM &&
        item_equals(&inv->slots[i].data.item, &item)) {
      /* Found matching item, stack it */
      int new_qty = inv->slots[i].quantity + quantity;
      if (new_qty > INVENTORY_STACK_LIMIT) {
        new_qty = INVENTORY_STACK_LIMIT;
      }
      inv->slots[i].quantity = new_qty;
      return RESULT_SUCCESS;
    }
  }

  /* No existing stack, find empty slot */
  for (int i = 0; i < INVENTORY_MAX_SLOTS; i++) {
    if (inv->slots[i].type == SLOT_EMPTY) {
      inv->slots[i].type = SLOT_ITEM;
      inv->slots[i].data.item = item;
      inv->slots[i].quantity = quantity;
      inv->used_slots++;
      return RESULT_SUCCESS;
    }
  }

  return RESULT_INVENTORY_FULL;
}

void inventory_remove_slot(Inventory *inv, int index) {
  if (!inv || index < 0 || index >= INVENTORY_MAX_SLOTS)
    return;

  if (inv->slots[index].type != SLOT_EMPTY) {
    inv->slots[index].type = SLOT_EMPTY;
    inv->slots[index].quantity = 0;
    inv->used_slots--;
  }
}

int inventory_use_item_at(Inventory *inv, int index) {
  if (!inv || index < 0 || index >= INVENTORY_MAX_SLOTS)
    return 0;

  if (inv->slots[index].type != SLOT_ITEM)
    return 0;
  if (inv->slots[index].quantity <= 0)
    return 0;

  inv->slots[index].quantity--;

  if (inv->slots[index].quantity == 0) {
    inventory_remove_slot(inv, index);
  }

  return 1;
}

InventorySlot *inventory_get_slot(Inventory *inv, int index) {
  if (!inv || index < 0 || index >= INVENTORY_MAX_SLOTS)
    return NULL;
  if (inv->slots[index].type == SLOT_EMPTY)
    return NULL;
  return &inv->slots[index];
}

int inventory_count_item(const Inventory *inv, const char *item_id) {
  if (!inv || !item_id)
    return 0;

  int total = 0;
  for (int i = 0; i < INVENTORY_MAX_SLOTS; i++) {
    if (inv->slots[i].type == SLOT_ITEM &&
        strcmp(inv->slots[i].data.item.id, item_id) == 0) {
      total += inv->slots[i].quantity;
    }
  }
  return total;
}

ResultCode inventory_remove_item_by_id(Inventory *inv, const char *item_id,
                                       int quantity) {
  if (!inv || !item_id || quantity <= 0)
    return RESULT_FAIL;

  int available = inventory_count_item(inv, item_id);
  if (available < quantity) {
    return RESULT_NOT_ENOUGH_MATERIALS;
  }

  int remaining = quantity;
  for (int i = 0; i < INVENTORY_MAX_SLOTS && remaining > 0; i++) {
    if (inv->slots[i].type == SLOT_ITEM &&
        strcmp(inv->slots[i].data.item.id, item_id) == 0) {

      if (inv->slots[i].quantity <= remaining) {
        remaining -= inv->slots[i].quantity;
        inventory_remove_slot(inv, i);
      } else {
        inv->slots[i].quantity -= remaining;
        remaining = 0;
      }
    }
  }

  return RESULT_SUCCESS;
}

int inventory_find_item(const Inventory *inv, const char *item_id) {
  if (!inv || !item_id)
    return -1;

  for (int i = 0; i < INVENTORY_MAX_SLOTS; i++) {
    if (inv->slots[i].type == SLOT_ITEM &&
        strcmp(inv->slots[i].data.item.id, item_id) == 0) {
      return i;
    }
  }
  return -1;
}

int inventory_is_full(const Inventory *inv) {
  if (!inv)
    return 1;
  return inv->used_slots >= INVENTORY_MAX_SLOTS;
}

int inventory_free_slots(const Inventory *inv) {
  if (!inv)
    return 0;
  return INVENTORY_MAX_SLOTS - inv->used_slots;
}

void inventory_print(const Inventory *inv) {
  if (!inv)
    return;

  printf("\n--- Inventory (%d/%d slots) ---\n", inv->used_slots,
         INVENTORY_MAX_SLOTS);

  int display_num = 1;
  for (int i = 0; i < INVENTORY_MAX_SLOTS; i++) {
    if (inv->slots[i].type == SLOT_EMPTY)
      continue;

    printf("%2d. ", display_num++);

    if (inv->slots[i].type == SLOT_WEAPON) {
      weapon_print(&inv->slots[i].data.weapon);
      printf("\n");
    } else if (inv->slots[i].type == SLOT_ITEM) {
      item_print(&inv->slots[i].data.item);
      if (inv->slots[i].quantity > 1) {
        printf(" x%d", inv->slots[i].quantity);
      }
      printf("\n");
    }
  }

  if (inv->used_slots == 0) {
    printf("  (Empty)\n");
  }
  printf("------------------------------\n");
}

void inventory_print_weapons(const Inventory *inv) {
  if (!inv)
    return;

  printf("\n--- Weapons ---\n");

  int display_num = 1;
  int found = 0;
  for (int i = 0; i < INVENTORY_MAX_SLOTS; i++) {
    if (inv->slots[i].type == SLOT_WEAPON) {
      printf("%2d. ", display_num++);
      weapon_print(&inv->slots[i].data.weapon);
      printf(" [Slot %d]\n", i);
      found = 1;
    }
  }

  if (!found) {
    printf("  (No weapons)\n");
  }
  printf("---------------\n");
}

void inventory_print_consumables(const Inventory *inv) {
  if (!inv)
    return;

  printf("\n--- Consumables ---\n");

  int display_num = 1;
  int found = 0;
  for (int i = 0; i < INVENTORY_MAX_SLOTS; i++) {
    if (inv->slots[i].type == SLOT_ITEM &&
        item_is_consumable(&inv->slots[i].data.item)) {
      printf("%2d. ", display_num++);
      item_print(&inv->slots[i].data.item);
      if (inv->slots[i].quantity > 1) {
        printf(" x%d", inv->slots[i].quantity);
      }
      printf(" [Slot %d]\n", i);
      found = 1;
    }
  }

  if (!found) {
    printf("  (No consumables)\n");
  }
  printf("-------------------\n");
}

void inventory_print_materials(const Inventory *inv) {
  if (!inv)
    return;

  printf("\n--- Materials ---\n");

  int display_num = 1;
  int found = 0;
  for (int i = 0; i < INVENTORY_MAX_SLOTS; i++) {
    if (inv->slots[i].type == SLOT_ITEM &&
        item_is_material(&inv->slots[i].data.item)) {
      printf("%2d. %s", display_num++, inv->slots[i].data.item.name);
      if (inv->slots[i].quantity > 1) {
        printf(" x%d", inv->slots[i].quantity);
      }
      printf("\n");
      found = 1;
    }
  }

  if (!found) {
    printf("  (No materials)\n");
  }
  printf("-----------------\n");
}
