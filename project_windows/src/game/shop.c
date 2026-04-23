/**
 * @file shop.c
 * @brief Shop system implementation
 */

#include "shop.h"
#include "../core/element.h"
#include "../core/item.h"
#include "../core/weapon.h"
#include "../data/registry.h"
#include "../utils/utils.h"
#include <stdio.h>
#include <string.h>

/* ============================================
 * INTERNAL STATE
 * ============================================ */

/* Track sold out items (runtime only) */
static int shop_stock_remaining[MAX_SHOP_ITEMS];
static int shop_initialized = 0;

static void init_shop_stock(void) {
  if (shop_initialized)
    return;

  int count;
  const ShopEntry *entries = registry_get_shop_entries(&count);

  for (int i = 0; i < count && i < MAX_SHOP_ITEMS; i++) {
    shop_stock_remaining[i] = entries[i].stock;
  }

  shop_initialized = 1;
}

/* ============================================
 * DISPLAY HELPERS
 * ============================================ */

static void shop_print_header(const Player *p) {
  clear_screen();
  print_header("SHOP", 50);
  printf("\n  Your Gold: %d\n\n", p->gold);
}

static void shop_print_weapons(void) {
  init_shop_stock();

  printf("=== WEAPONS ===\n");

  int count;
  const ShopEntry *entries = registry_get_shop_entries(&count);

  int display_num = 1;
  for (int i = 0; i < count; i++) {
    if (!entries[i].is_weapon)
      continue;

    const WeaponTemplate *w = registry_get_weapon(entries[i].item_id);
    if (!w || w->buy_price <= 0)
      continue;

    /* Check stock */
    int stock = shop_stock_remaining[i];
    if (stock == 0) {
      printf("  %2d. [SOLD OUT] %s\n", display_num++, w->name);
      continue;
    }

    printf("  %2d. %s - %d G", display_num++, w->name, w->buy_price);
    printf(" | DMG: %d", w->base_damage);
    if (w->element != ELEMENT_NONE) {
      printf(" (%s)", element_to_string(w->element));
    }
    if (stock > 0) {
      printf(" [Stock: %d]", stock);
    }
    printf("\n");
  }
  printf("\n");
}

static void shop_print_items(void) {
  init_shop_stock();

  printf("=== ITEMS ===\n");

  int count;
  const ShopEntry *entries = registry_get_shop_entries(&count);

  int display_num = 1;
  for (int i = 0; i < count; i++) {
    if (entries[i].is_weapon)
      continue;

    const ItemTemplate *it = registry_get_item(entries[i].item_id);
    if (!it || it->buy_price <= 0)
      continue;

    int stock = shop_stock_remaining[i];
    if (stock == 0) {
      printf("  %2d. [SOLD OUT] %s\n", display_num++, it->name);
      continue;
    }

    printf("  %2d. %s - %d G", display_num++, it->name, it->buy_price);

    /* Show effect for consumables */
    if (it->category == ITEM_CATEGORY_CONSUMABLE) {
      switch (it->consumable_type) {
      case CONSUMABLE_HEAL:
        printf(" (+%d HP)", it->value);
        break;
      case CONSUMABLE_BUFF_ATK:
        printf(" (+%d%% ATK, %dt)", it->value, it->duration);
        break;
      case CONSUMABLE_BUFF_DEF:
        printf(" (+%d%% DEF, %dt)", it->value, it->duration);
        break;
      default:
        break;
      }
    }

    if (stock > 0) {
      printf(" [Stock: %d]", stock);
    }
    printf("\n");
  }
  printf("\n");
}

static void shop_menu(void) {
  printf("--- Shop Menu ---\n");
  printf("1. Buy Weapons\n");
  printf("2. Buy Items\n");
  printf("3. Sell Items\n");
  printf("4. Exit Shop\n");
  printf("Choice: ");
}

/* ============================================
 * BUY/SELL LOGIC
 * ============================================ */

static void buy_weapon(Player *p) {
  init_shop_stock();

  clear_screen();
  print_header("BUY WEAPONS", 50);
  printf("\n  Your Gold: %d\n\n", p->gold);
  shop_print_weapons();

  printf("Enter number to buy (0 to cancel): ");
  int choice = get_int_input(0, 100);

  if (choice == 0)
    return;

  /* Find the weapon by display number */
  int count;
  const ShopEntry *entries = registry_get_shop_entries(&count);

  int display_num = 0;
  int entry_index = -1;
  for (int i = 0; i < count; i++) {
    if (!entries[i].is_weapon)
      continue;

    const WeaponTemplate *w = registry_get_weapon(entries[i].item_id);
    if (!w || w->buy_price <= 0)
      continue;

    display_num++;
    if (display_num == choice) {
      entry_index = i;
      break;
    }
  }

  if (entry_index < 0) {
    printf("Invalid choice.\n");
    delay_ms(DELAY_MENU_TRANSITION);
    return;
  }

  /* Check stock */
  if (shop_stock_remaining[entry_index] == 0) {
    printf("That item is sold out!\n");
    delay_ms(DELAY_MENU_TRANSITION);
    return;
  }

  const WeaponTemplate *w = registry_get_weapon(entries[entry_index].item_id);

  /* Check gold */
  if (p->gold < w->buy_price) {
    printf("Not enough gold! Need %d, have %d.\n", w->buy_price, p->gold);
    delay_ms(DELAY_MENU_TRANSITION);
    return;
  }

  /* Check inventory */
  if (inventory_is_full(&p->inventory)) {
    printf("Inventory is full!\n");
    delay_ms(DELAY_MENU_TRANSITION);
    return;
  }

  /* Purchase */
  Weapon new_weapon = weapon_create(w->id);
  ResultCode result = inventory_add_weapon(&p->inventory, new_weapon);

  if (result == RESULT_SUCCESS) {
    player_spend_gold(p, w->buy_price);

    /* Reduce stock if limited */
    if (shop_stock_remaining[entry_index] > 0) {
      shop_stock_remaining[entry_index]--;
    }

    printf("Purchased %s!\n", w->name);
  } else {
    printf("Failed to purchase.\n");
  }

  delay_ms(DELAY_MENU_TRANSITION);
}

static void buy_item(Player *p) {
  init_shop_stock();

  clear_screen();
  print_header("BUY ITEMS", 50);
  printf("\n  Your Gold: %d\n\n", p->gold);
  shop_print_items();

  printf("Enter number to buy (0 to cancel): ");
  int choice = get_int_input(0, 100);

  if (choice == 0)
    return;

  /* Find the item by display number */
  int count;
  const ShopEntry *entries = registry_get_shop_entries(&count);

  int display_num = 0;
  int entry_index = -1;
  for (int i = 0; i < count; i++) {
    if (entries[i].is_weapon)
      continue;

    const ItemTemplate *it = registry_get_item(entries[i].item_id);
    if (!it || it->buy_price <= 0)
      continue;

    display_num++;
    if (display_num == choice) {
      entry_index = i;
      break;
    }
  }

  if (entry_index < 0) {
    printf("Invalid choice.\n");
    delay_ms(DELAY_MENU_TRANSITION);
    return;
  }

  if (shop_stock_remaining[entry_index] == 0) {
    printf("That item is sold out!\n");
    delay_ms(DELAY_MENU_TRANSITION);
    return;
  }

  const ItemTemplate *it = registry_get_item(entries[entry_index].item_id);

  /* Ask quantity */
  printf("How many? (Max affordable: %d): ", p->gold / it->buy_price);
  int qty = get_int_input(1, 99);
  if (qty < 1) {
    return;
  }

  /* Check limited stock */
  if (shop_stock_remaining[entry_index] > 0 &&
      qty > shop_stock_remaining[entry_index]) {
    qty = shop_stock_remaining[entry_index];
    printf("Only %d available.\n", qty);
  }

  int total_cost = it->buy_price * qty;

  if (p->gold < total_cost) {
    printf("Not enough gold! Need %d, have %d.\n", total_cost, p->gold);
    delay_ms(DELAY_MENU_TRANSITION);
    return;
  }

  /* Purchase */
  Item new_item = item_create(it->id);
  ResultCode result = inventory_add_item(&p->inventory, new_item, qty);

  if (result == RESULT_SUCCESS) {
    player_spend_gold(p, total_cost);

    if (shop_stock_remaining[entry_index] > 0) {
      shop_stock_remaining[entry_index] -= qty;
    }

    printf("Purchased %s x%d!\n", it->name, qty);
  } else if (result == RESULT_INVENTORY_FULL) {
    printf("Inventory is full!\n");
  } else {
    printf("Failed to purchase.\n");
  }

  delay_ms(DELAY_MENU_TRANSITION);
}

static void sell_items(Player *p) {
  clear_screen();
  print_header("SELL ITEMS", 50);
  printf("\n  Your Gold: %d\n\n", p->gold);

  inventory_print(&p->inventory);

  printf("\nEnter slot number to sell (0 to cancel): ");
  int slot = get_int_input(0, INVENTORY_MAX_SLOTS);

  if (slot == 0)
    return;

  /* Find actual slot index */
  int actual_slot = -1;
  int display_num = 0;
  for (int i = 0; i < INVENTORY_MAX_SLOTS; i++) {
    InventorySlot *inv_slot = inventory_get_slot(&p->inventory, i);
    if (inv_slot) {
      display_num++;
      if (display_num == slot) {
        actual_slot = i;
        break;
      }
    }
  }

  if (actual_slot < 0) {
    printf("Invalid slot.\n");
    delay_ms(DELAY_MENU_TRANSITION);
    return;
  }

  InventorySlot *inv_slot = inventory_get_slot(&p->inventory, actual_slot);
  if (!inv_slot) {
    printf("Empty slot.\n");
    delay_ms(DELAY_MENU_TRANSITION);
    return;
  }

  int sell_price = 0;
  const char *item_name = "";
  int qty = 1;

  if (inv_slot->type == SLOT_WEAPON) {
    sell_price = inv_slot->data.weapon.sell_price;
    item_name = inv_slot->data.weapon.name;
  } else if (inv_slot->type == SLOT_ITEM) {
    sell_price = inv_slot->data.item.sell_price;
    item_name = inv_slot->data.item.name;
    qty = inv_slot->quantity;

    if (qty > 1) {
      printf("Sell how many? (1-%d): ", qty);
      qty = get_int_input(1, qty);
      if (qty < 1)
        return;
    }

    sell_price *= qty;
  }

  if (sell_price <= 0) {
    printf("That item cannot be sold.\n");
    delay_ms(DELAY_MENU_TRANSITION);
    return;
  }

  printf("Sell %s", item_name);
  if (qty > 1)
    printf(" x%d", qty);
  printf(" for %d gold?", sell_price);

  if (get_yes_no()) {
    if (inv_slot->type == SLOT_WEAPON) {
      inventory_remove_slot(&p->inventory, actual_slot);
    } else {
      for (int i = 0; i < qty; i++) {
        inventory_use_item_at(&p->inventory, actual_slot);
      }
    }

    player_add_gold(p, sell_price);
    printf("Sold for %d gold!\n", sell_price);
  } else {
    printf("Cancelled.\n");
  }

  delay_ms(DELAY_MENU_TRANSITION);
}

/* ============================================
 * PUBLIC FUNCTIONS
 * ============================================ */

MenuResult shop_enter(Player *player) {
  if (!player)
    return MENU_BACK;

  init_shop_stock();

  while (1) {
    shop_print_header(player);
    shop_menu();

    int choice = get_int_input(1, 4);

    switch (choice) {
    case 1:
      buy_weapon(player);
      break;
    case 2:
      buy_item(player);
      break;
    case 3:
      sell_items(player);
      break;
    case 4:
      return MENU_BACK;
    default:
      printf("Invalid choice.\n");
      delay_ms(DELAY_MENU_TRANSITION);
    }
  }
}
