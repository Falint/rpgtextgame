/**
 * @file main.c
 * @brief TextRPG Main Entry Point
 *
 * A text-based RPG game with:
 * - Turn-based combat with elemental system
 * - Monster loot drops
 * - Weapon upgrade system
 * - Shop with buy/sell
 * - Buff potions and consumables
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "config.h"
#include "core/enemy.h"
#include "core/inventory.h"
#include "core/player.h"
#include "data/registry.h"
#include "game/battle.h"
#include "game/shop.h"
#include "game/upgrade.h"
#include "types.h"
#include "utils/utils.h"

/* ============================================
 * MENU DISPLAY
 * ============================================ */

static void print_main_menu(void) {
  printf("\n=== MAIN MENU ===\n");
  printf("1. Battle (Normal)\n");
  printf("2. Battle (Elite)\n");
  printf("3. Battle (BOSS)\n");
  printf("4. Shop\n");
  printf("5. Upgrade Weapons\n");
  printf("6. Equip Weapon\n");
  printf("7. Inventory\n");
  printf("8. Status\n");
  printf("9. Exit\n");
  printf("=================\n");
  printf("Choice: ");
}

static void equip_menu(Player *p) {
  clear_screen();
  print_header("EQUIP WEAPON", 40);

  printf("\nCurrently Equipped: ");
  if (p->has_weapon) {
    weapon_print(&p->equipped_weapon);
  } else {
    printf("None");
  }
  printf("\n");

  inventory_print_weapons(&p->inventory);

  printf("\nEnter slot number to equip (0 to cancel): ");
  int choice = get_int_input(0, INVENTORY_MAX_SLOTS);

  if (choice == 0)
    return;

  /* Find actual slot index */
  int actual_slot = -1;
  int display_num = 0;
  for (int i = 0; i < INVENTORY_MAX_SLOTS; i++) {
    InventorySlot *slot = inventory_get_slot(&p->inventory, i);
    if (slot && slot->type == SLOT_WEAPON) {
      display_num++;
      if (display_num == choice) {
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

  ResultCode result = player_equip_weapon(p, actual_slot);
  if (result == RESULT_SUCCESS) {
    printf("Equipped ");
    weapon_print(&p->equipped_weapon);
    printf("!\n");
  } else {
    printf("Failed to equip weapon.\n");
  }

  delay_ms(DELAY_MENU_TRANSITION);
}

static void inventory_menu(Player *p) {
  clear_screen();
  print_header("INVENTORY", 40);

  inventory_print(&p->inventory);

  printf("\n1. View Weapons\n");
  printf("2. View Consumables\n");
  printf("3. View Materials\n");
  printf("4. Use Item\n");
  printf("5. Back\n");
  printf("Choice: ");

  int choice = get_int_input(1, 5);

  switch (choice) {
  case 1:
    clear_screen();
    inventory_print_weapons(&p->inventory);
    wait_for_enter();
    break;
  case 2:
    clear_screen();
    inventory_print_consumables(&p->inventory);
    wait_for_enter();
    break;
  case 3:
    clear_screen();
    inventory_print_materials(&p->inventory);
    wait_for_enter();
    break;
  case 4: {
    inventory_print_consumables(&p->inventory);
    printf("\nEnter slot to use (0 to cancel): ");
    int slot = get_int_input(0, INVENTORY_MAX_SLOTS);

    if (slot > 0) {
      /* Find actual slot */
      int actual_slot = -1;
      int display_num = 0;
      for (int i = 0; i < INVENTORY_MAX_SLOTS; i++) {
        InventorySlot *inv_slot = inventory_get_slot(&p->inventory, i);
        if (inv_slot && inv_slot->type == SLOT_ITEM &&
            item_is_consumable(&inv_slot->data.item)) {
          display_num++;
          if (display_num == slot) {
            actual_slot = i;
            break;
          }
        }
      }

      if (actual_slot >= 0) {
        player_use_item(p, actual_slot);
      } else {
        printf("Invalid slot.\n");
      }
      delay_ms(DELAY_MENU_TRANSITION);
    }
    break;
  }
  default:
    break;
  }
}

static void status_menu(const Player *p) {
  clear_screen();
  player_print_detailed(p);
  wait_for_enter();
}

/* ============================================
 * GAME LOOP
 * ============================================ */

static void game_loop(Player *player) {
  int running = 1;

  while (running && player_is_alive(player)) {
    clear_screen();
    player_print_status(player);
    print_main_menu();

    int choice = get_int_input(1, 9);

    switch (choice) {
    case 1: {
      /* Battle Normal */
      BattleResult result = battle_random(player, MONSTER_TYPE_NORMAL);
      if (result == BATTLE_LOSE) {
        running = 0;
      }
      break;
    }

    case 2: {
      /* Battle Elite */
      printf("\nElite monsters are dangerous! Continue?");
      if (get_yes_no()) {
        BattleResult result = battle_random(player, MONSTER_TYPE_ELITE);
        if (result == BATTLE_LOSE) {
          running = 0;
        }
      }
      break;
    }

    case 3: {
      /* Battle Boss */
      printf("\n>>> BOSS BATTLE! You cannot escape! <<<\n");
      printf("Are you ready?");
      if (get_yes_no()) {
        BattleResult result = battle_random(player, MONSTER_TYPE_BOSS);
        if (result == BATTLE_LOSE) {
          running = 0;
        }
      }
      break;
    }

    case 4:
      shop_enter(player);
      break;

    case 5:
      upgrade_enter(player);
      break;

    case 6:
      equip_menu(player);
      break;

    case 7:
      inventory_menu(player);
      break;

    case 8:
      status_menu(player);
      break;

    case 9:
      printf("\nAre you sure you want to exit?");
      if (get_yes_no()) {
        running = 0;
      }
      break;

    default:
      printf("Invalid choice.\n");
      delay_ms(DELAY_MENU_TRANSITION);
    }
  }
}

/* ============================================
 * MAIN
 * ============================================ */

int main(void) {
  /* Initialize random seed */
  srand((unsigned int)time(NULL));

  /* Initialize data registry */
  registry_init();

  /* Title screen */
  clear_screen();
  print_header("TEXT RPG", 50);
  printf("\n");
  print_centered("A Data-Driven Adventure", 50);
  printf("\n");
  print_separator(50, '-');
  printf("\n");

  /* Get player name */
  printf("Enter your name: ");
  char name[MAX_NAME_LEN];
  if (scanf("%31s", name) != 1) {
    safe_strcpy(name, "Hero", MAX_NAME_LEN);
  }
  clear_input_buffer();

  /* Initialize player */
  Player player;
  player_init(&player, name);

  /* Welcome message */
  clear_screen();
  printf("\nWelcome, %s!\n\n", player.name);
  printf("You start with:\n");
  printf("  - %d HP\n", player.max_hp);
  printf("  - %d Gold\n", player.gold);
  printf("  - 3x Small Potions\n");
  printf("\nGood luck on your adventure!\n");

  wait_for_enter();

  /* Main game loop */
  game_loop(&player);

  /* Game over / Exit screen */
  clear_screen();
  print_header("GAME OVER", 50);
  printf("\n");

  if (!player_is_alive(&player)) {
    printf("You have been defeated.\n");
  } else {
    printf("Thanks for playing!\n");
  }

  printf("\nFinal Stats:\n");
  printf("  Gold earned: %d\n", player.gold);
  printf("  Items collected: %d\n", player.inventory.used_slots);

  if (player.has_weapon && strcmp(player.equipped_weapon.id, "fists") != 0) {
    printf("  Final weapon: ");
    weapon_print(&player.equipped_weapon);
    printf("\n");
  }

  printf("\n");

  return 0;
}
