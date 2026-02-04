/**
 * @file battle.c
 * @brief Battle system implementation
 */

#include "battle.h"
#include "../core/element.h"
#include "../core/loot.h"
#include "../utils/utils.h"
#include <stdio.h>

/* ============================================
 * INTERNAL HELPERS
 * ============================================ */

static void battle_print_header(const Player *p, const Enemy *e) {
  clear_screen();
  print_header("BATTLE!", 40);
  printf("\n");
  player_print_status(p);
  printf("\n");
  enemy_print_status(e);
  printf("\n");
}

static void battle_menu(void) {
  printf("\n--- Actions ---\n");
  printf("1. Attack\n");
  printf("2. Use Item\n");
  printf("3. Run\n");
  printf("Choice: ");
}

static int calculate_damage(const Player *p, const Enemy *e) {
  int base_damage = player_get_attack(p);

  /* Apply element modifier */
  Element weapon_element = ELEMENT_NONE;
  if (p->has_weapon) {
    weapon_element = p->equipped_weapon.element;
  }

  float modifier = element_get_modifier(weapon_element, e->element);
  int damage = (int)(base_damage * modifier);

  /* Apply enemy defense */
  damage = damage - (int)(e->defense * DEF_REDUCTION_FACTOR);
  if (damage < 1) {
    damage = 1;
  }

  return damage;
}

static void player_attack(Player *p, Enemy *e) {
  int damage = calculate_damage(p, e);

  /* Check for critical hit */
  int is_crit = random_chance(CRIT_CHANCE_BASE);
  if (is_crit) {
    damage = (int)(damage * CRIT_DAMAGE_MULT);
    printf("\n>>> CRITICAL HIT! <<<\n");
  }

  /* Apply damage */
  printf("You attack %s for %d damage!", e->name, damage);

  /* Show element effectiveness */
  Element weapon_element =
      p->has_weapon ? p->equipped_weapon.element : ELEMENT_NONE;
  float mod = element_get_modifier(weapon_element, e->element);
  if (mod > 1.0f) {
    printf(" (Super Effective!)");
  } else if (mod < 1.0f) {
    printf(" (Not Very Effective...)");
  }
  printf("\n");

  /* Actually apply damage to enemy directly (bypass defense since we calculated
   * it) */
  e->current_hp -= damage;
  if (e->current_hp < 0) {
    e->current_hp = 0;
  }

  delay_ms(DELAY_AFTER_ATTACK);
}

static void enemy_attack(Enemy *e, Player *p) {
  printf("\n--- %s's Turn ---\n", e->name);
  delay_ms(DELAY_TURN_CHANGE);

  int damage = enemy_calculate_damage(e);

  printf("%s attacks you for %d damage!\n", e->name, damage);

  int died = player_take_damage(p, damage);
  printf("You take %d damage after defense.\n",
         damage - (int)(player_get_defense(p) * DEF_REDUCTION_FACTOR));

  if (died) {
    printf("\nYou have been defeated...\n");
  }

  delay_ms(DELAY_AFTER_ATTACK);
}

static int use_item_menu(Player *p) {
  inventory_print_consumables(&p->inventory);
  printf("\nEnter slot number (0 to cancel): ");

  int slot = get_int_input(0, INVENTORY_MAX_SLOTS - 1);
  if (slot <= 0) {
    return 0; /* Cancelled */
  }

  /* Convert display number to actual slot */
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

  if (actual_slot < 0) {
    printf("Invalid slot.\n");
    delay_ms(DELAY_MENU_TRANSITION);
    return 0;
  }

  ResultCode result = player_use_item(p, actual_slot);
  if (result != RESULT_SUCCESS) {
    printf("Cannot use that item.\n");
  }

  delay_ms(DELAY_AFTER_ATTACK);
  return 1; /* Turn used */
}

static int try_escape(void) {
  /* 40% base escape chance */
  return random_chance(0.4f);
}

/* ============================================
 * PUBLIC FUNCTIONS
 * ============================================ */

BattleResult battle_start(Player *player, Enemy *enemy) {
  if (!player || !enemy) {
    return BATTLE_LOSE;
  }

  /* Battle intro */
  clear_screen();
  print_header("BATTLE START!", 40);
  printf("\n  A wild %s appeared!\n", enemy->name);
  if (enemy->type == MONSTER_TYPE_ELITE) {
    printf("  >> This is an ELITE monster! <<\n");
  } else if (enemy->type == MONSTER_TYPE_BOSS) {
    printf("  >>> BOSS BATTLE! <<<\n");
  }
  delay_ms(DELAY_BATTLE_START);

  /* Main battle loop */
  while (player_is_alive(player) && enemy_is_alive(enemy)) {
    battle_print_header(player, enemy);
    battle_menu();

    int choice = get_int_input(1, 3);

    switch (choice) {
    case 1: /* Attack */
      player_attack(player, enemy);
      break;

    case 2: /* Use Item */
      if (!use_item_menu(player)) {
        continue; /* Turn not used */
      }
      break;

    case 3: /* Run */
      if (enemy->type == MONSTER_TYPE_BOSS) {
        printf("\nYou can't escape from a boss!\n");
        delay_ms(DELAY_MENU_TRANSITION);
        continue;
      }

      if (try_escape()) {
        printf("\nYou escaped successfully!\n");
        delay_ms(DELAY_BATTLE_END);
        return BATTLE_ESCAPE;
      } else {
        printf("\nCouldn't escape!\n");
        delay_ms(DELAY_AFTER_ATTACK);
      }
      break;

    default:
      printf("Invalid choice.\n");
      delay_ms(DELAY_MENU_TRANSITION);
      continue;
    }

    /* Check if enemy died */
    if (!enemy_is_alive(enemy)) {
      break;
    }

    /* Enemy turn */
    enemy_attack(enemy, player);

    /* Tick buffs at end of turn */
    player_tick_buffs(player);
  }

  /* Battle end */
  clear_screen();

  if (player_is_alive(player)) {
    /* Victory! */
    print_header("VICTORY!", 40);
    printf("\nYou defeated %s!\n", enemy->name);

    /* Generate and collect loot */
    LootDrop loot = loot_generate(enemy);
    loot_print(&loot);
    loot_collect(player, &loot);

    delay_ms(DELAY_BATTLE_END);
    return BATTLE_WIN;
  } else {
    /* Defeat */
    print_header("DEFEAT", 40);
    printf("\nYou were defeated by %s...\n", enemy->name);
    delay_ms(DELAY_BATTLE_END);
    return BATTLE_LOSE;
  }
}

BattleResult battle_random(Player *player, MonsterType type) {
  Enemy enemy = enemy_create_random(type);
  return battle_start(player, &enemy);
}
