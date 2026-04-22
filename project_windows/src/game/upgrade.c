/**
 * @file upgrade.c
 * @brief Weapon upgrade system implementation
 */

#include "upgrade.h"
#include "../core/inventory.h"
#include "../core/weapon.h"
#include "../data/registry.h"
#include "../utils/utils.h"
#include <stdio.h>
#include <string.h>

/* ============================================
 * INTERNAL HELPERS
 * ============================================ */

static const UpgradeRecipe *get_recipe_for_weapon(const Weapon *w) {
  if (!w)
    return NULL;
  return registry_get_upgrade(w->id, w->tier);
}

static int check_materials(const Inventory *inv, const UpgradeRecipe *recipe) {
  if (!inv || !recipe)
    return 0;

  for (int i = 0; i < recipe->material_count; i++) {
    int have = inventory_count_item(inv, recipe->materials[i].item_id);
    if (have < recipe->materials[i].quantity) {
      return 0;
    }
  }
  return 1;
}

static void consume_materials(Inventory *inv, const UpgradeRecipe *recipe) {
  if (!inv || !recipe)
    return;

  for (int i = 0; i < recipe->material_count; i++) {
    inventory_remove_item_by_id(inv, recipe->materials[i].item_id,
                                recipe->materials[i].quantity);
  }
}

/* ============================================
 * DISPLAY
 * ============================================ */

void upgrade_print_requirements(const Weapon *weapon, const Player *player) {
  if (!weapon)
    return;

  const UpgradeRecipe *recipe = get_recipe_for_weapon(weapon);

  if (!recipe) {
    if (weapon->tier >= weapon->max_tier) {
      printf("  This weapon is at maximum tier.\n");
    } else {
      printf("  No upgrade path available.\n");
    }
    return;
  }

  printf("  Upgrade: %s +%d -> +%d\n", weapon->name, weapon->tier - 1,
         recipe->to_tier - 1);
  printf("  Damage: %d -> %d (+%d)\n", weapon_get_damage(weapon),
         weapon_get_damage(weapon) + recipe->damage_bonus,
         recipe->damage_bonus);
  printf("\n  Requirements:\n");
  printf("    Gold: %d", recipe->gold_cost);
  if (player && player->gold >= recipe->gold_cost) {
    printf(" [OK]");
  } else {
    printf(" [NEED %d more]", recipe->gold_cost - (player ? player->gold : 0));
  }
  printf("\n");

  for (int i = 0; i < recipe->material_count; i++) {
    const ItemTemplate *item = registry_get_item(recipe->materials[i].item_id);
    const char *name = item ? item->name : recipe->materials[i].item_id;
    int have = player ? inventory_count_item(&player->inventory,
                                             recipe->materials[i].item_id)
                      : 0;
    int need = recipe->materials[i].quantity;

    printf("    %s: %d/%d", name, have, need);
    if (have >= need) {
      printf(" [OK]");
    } else {
      printf(" [NEED %d more]", need - have);
    }
    printf("\n");
  }
}

static void print_upgradeable_weapons(const Player *p) {
  printf("=== Upgradeable Weapons ===\n\n");

  int found = 0;

  /* Check equipped weapon */
  if (p->has_weapon && weapon_can_upgrade(&p->equipped_weapon)) {
    printf("  [E] ");
    weapon_print(&p->equipped_weapon);
    printf(" (EQUIPPED)\n");

    const UpgradeRecipe *recipe = get_recipe_for_weapon(&p->equipped_weapon);
    if (recipe) {
      int can_upgrade = p->gold >= recipe->gold_cost &&
                        check_materials(&p->inventory, recipe);
      printf("      -> Tier %d (%s)\n", recipe->to_tier,
             can_upgrade ? "READY" : "missing materials");
    }
    found = 1;
  }

  /* Check inventory weapons */
  int display_num = 1;
  for (int i = 0; i < INVENTORY_MAX_SLOTS; i++) {
    InventorySlot *slot = inventory_get_slot((Inventory *)&p->inventory, i);
    if (!slot || slot->type != SLOT_WEAPON)
      continue;

    Weapon *w = &slot->data.weapon;
    if (!weapon_can_upgrade(w))
      continue;

    printf("  %2d. ", display_num++);
    weapon_print(w);
    printf(" [Slot %d]\n", i);

    const UpgradeRecipe *recipe = get_recipe_for_weapon(w);
    if (recipe) {
      int can_upgrade = p->gold >= recipe->gold_cost &&
                        check_materials(&p->inventory, recipe);
      printf("      -> Tier %d (%s)\n", recipe->to_tier,
             can_upgrade ? "READY" : "missing materials");
    }
    found = 1;
  }

  if (!found) {
    printf("  No weapons can be upgraded.\n");
    printf("  (Collect materials from monsters or buy upgradeable weapons)\n");
  }

  printf("\n");
}

/* ============================================
 * PUBLIC FUNCTIONS
 * ============================================ */

int upgrade_can_upgrade_weapon(const Player *player, int weapon_slot) {
  if (!player)
    return 0;

  const Weapon *w;
  if (weapon_slot < 0) {
    /* Equipped weapon */
    if (!player->has_weapon)
      return 0;
    w = &player->equipped_weapon;
  } else {
    InventorySlot *slot =
        inventory_get_slot((Inventory *)&player->inventory, weapon_slot);
    if (!slot || slot->type != SLOT_WEAPON)
      return 0;
    w = &slot->data.weapon;
  }

  if (!weapon_can_upgrade(w))
    return 0;

  const UpgradeRecipe *recipe = get_recipe_for_weapon(w);
  if (!recipe)
    return 0;

  if (player->gold < recipe->gold_cost)
    return 0;
  if (!check_materials(&player->inventory, recipe))
    return 0;

  return 1;
}

ResultCode upgrade_weapon(Player *player, int weapon_slot) {
  if (!player)
    return RESULT_FAIL;

  Weapon *w;
  if (weapon_slot < 0) {
    if (!player->has_weapon)
      return RESULT_INVALID_SLOT;
    w = &player->equipped_weapon;
  } else {
    InventorySlot *slot = inventory_get_slot(&player->inventory, weapon_slot);
    if (!slot || slot->type != SLOT_WEAPON)
      return RESULT_INVALID_SLOT;
    w = &slot->data.weapon;
  }

  if (!weapon_can_upgrade(w)) {
    return RESULT_MAX_TIER_REACHED;
  }

  const UpgradeRecipe *recipe = get_recipe_for_weapon(w);
  if (!recipe) {
    return RESULT_CANNOT_UPGRADE;
  }

  /* Check gold */
  if (player->gold < recipe->gold_cost) {
    return RESULT_NOT_ENOUGH_GOLD;
  }

  /* Check materials */
  if (!check_materials(&player->inventory, recipe)) {
    return RESULT_NOT_ENOUGH_MATERIALS;
  }

  /* Perform upgrade */
  player_spend_gold(player, recipe->gold_cost);
  consume_materials(&player->inventory, recipe);
  weapon_apply_upgrade(w, recipe->damage_bonus);

  return RESULT_SUCCESS;
}

MenuResult upgrade_enter(Player *player) {
  if (!player)
    return MENU_BACK;

  while (1) {
    clear_screen();
    print_header("WEAPON UPGRADE", 50);
    printf("\n  Your Gold: %d\n\n", player->gold);

    print_upgradeable_weapons(player);

    /* Show materials */
    inventory_print_materials(&player->inventory);

    printf("\n--- Upgrade Menu ---\n");
    printf("1. Upgrade Equipped Weapon\n");
    printf("2. Upgrade Inventory Weapon\n");
    printf("3. View Details\n");
    printf("4. Back\n");
    printf("Choice: ");

    int choice = get_int_input(1, 4);

    switch (choice) {
    case 1: {
      /* Upgrade equipped */
      if (!player->has_weapon) {
        printf("No weapon equipped.\n");
        delay_ms(DELAY_MENU_TRANSITION);
        break;
      }

      printf("\n");
      upgrade_print_requirements(&player->equipped_weapon, player);
      printf("\nUpgrade?");

      if (get_yes_no()) {
        ResultCode result = upgrade_weapon(player, -1);
        switch (result) {
        case RESULT_SUCCESS:
          printf("\nUpgrade successful!\n");
          weapon_print(&player->equipped_weapon);
          printf("\n");
          break;
        case RESULT_NOT_ENOUGH_GOLD:
          printf("\nNot enough gold!\n");
          break;
        case RESULT_NOT_ENOUGH_MATERIALS:
          printf("\nNot enough materials!\n");
          break;
        case RESULT_MAX_TIER_REACHED:
          printf("\nWeapon is at max tier!\n");
          break;
        default:
          printf("\nUpgrade failed.\n");
        }
      }
      delay_ms(DELAY_MENU_TRANSITION);
      break;
    }

    case 2: {
      /* Upgrade from inventory */
      printf("\nEnter weapon slot number (0 to cancel): ");
      int slot_num = get_int_input(0, INVENTORY_MAX_SLOTS);

      if (slot_num == 0)
        break;

      /* Find actual slot */
      int actual_slot = -1;
      int display_num = 0;
      for (int i = 0; i < INVENTORY_MAX_SLOTS; i++) {
        InventorySlot *slot = inventory_get_slot(&player->inventory, i);
        if (slot && slot->type == SLOT_WEAPON &&
            weapon_can_upgrade(&slot->data.weapon)) {
          display_num++;
          if (display_num == slot_num) {
            actual_slot = i;
            break;
          }
        }
      }

      if (actual_slot < 0) {
        printf("Invalid slot.\n");
        delay_ms(DELAY_MENU_TRANSITION);
        break;
      }

      InventorySlot *slot = inventory_get_slot(&player->inventory, actual_slot);
      printf("\n");
      upgrade_print_requirements(&slot->data.weapon, player);
      printf("\nUpgrade?");

      if (get_yes_no()) {
        ResultCode result = upgrade_weapon(player, actual_slot);
        if (result == RESULT_SUCCESS) {
          printf("\nUpgrade successful!\n");
          weapon_print(&slot->data.weapon);
          printf("\n");
        } else {
          printf("\nUpgrade failed.\n");
        }
      }
      delay_ms(DELAY_MENU_TRANSITION);
      break;
    }

    case 3: {
      /* View details */
      printf("\nEnter 'E' for equipped or slot number: ");
      char input[10];
      if (scanf("%9s", input) != 1) {
        clear_input_buffer();
        break;
      }

      if (input[0] == 'E' || input[0] == 'e') {
        if (player->has_weapon) {
          printf("\n");
          weapon_print_detailed(&player->equipped_weapon);
          printf("\n");
          upgrade_print_requirements(&player->equipped_weapon, player);
        }
      }

      wait_for_enter();
      break;
    }

    case 4:
      return MENU_BACK;

    default:
      break;
    }
  }
}
