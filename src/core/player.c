/**
 * @file player.c
 * @brief Player system implementation
 */

#include "player.h"
#include "../utils/utils.h"
#include "element.h"
#include <stdio.h>
#include <string.h>

void player_init(Player *p, const char *name) {
  if (!p)
    return;

  safe_strcpy(p->name, name, MAX_NAME_LEN);
  p->max_hp = PLAYER_STARTING_HP;
  p->current_hp = PLAYER_STARTING_HP;
  p->base_attack = 5;
  p->base_defense = PLAYER_STARTING_DEF;
  p->gold = PLAYER_STARTING_GOLD;

  inventory_init(&p->inventory);

  /* Start with fists */
  p->equipped_weapon = weapon_create("fists");
  p->has_weapon = 1;

  /* No active buffs */
  p->buff_count = 0;
  for (int i = 0; i < BUFF_MAX_ACTIVE; i++) {
    p->active_buffs[i].type = CONSUMABLE_NONE;
    p->active_buffs[i].value = 0;
    p->active_buffs[i].turns_remaining = 0;
  }

  /* Give starter items */
  Item potion = item_create("small_potion");
  inventory_add_item(&p->inventory, potion, 3);
}

int player_get_attack(const Player *p) {
  if (!p)
    return 0;

  int total = p->base_attack;

  if (p->has_weapon) {
    total += weapon_get_damage(&p->equipped_weapon);
  }

  /* Apply ATK buffs */
  for (int i = 0; i < p->buff_count; i++) {
    if (p->active_buffs[i].type == CONSUMABLE_BUFF_ATK) {
      total = (int)(total * (1.0f + p->active_buffs[i].value / 100.0f));
    }
  }

  return total;
}

int player_get_defense(const Player *p) {
  if (!p)
    return 0;

  int total = p->base_defense;

  /* Apply DEF buffs */
  for (int i = 0; i < p->buff_count; i++) {
    if (p->active_buffs[i].type == CONSUMABLE_BUFF_DEF) {
      total = (int)(total * (1.0f + p->active_buffs[i].value / 100.0f));
    }
  }

  return total;
}

ResultCode player_equip_weapon(Player *p, int slot) {
  if (!p)
    return RESULT_FAIL;

  InventorySlot *inv_slot = inventory_get_slot(&p->inventory, slot);
  if (!inv_slot || inv_slot->type != SLOT_WEAPON) {
    return RESULT_INVALID_SLOT;
  }

  Weapon new_weapon = inv_slot->data.weapon;

  /* Return old weapon to inventory (if not fists) */
  if (p->has_weapon && strcmp(p->equipped_weapon.id, "fists") != 0) {
    Weapon old_weapon = p->equipped_weapon;
    /* Put old weapon in the slot we're taking from */
    inv_slot->data.weapon = old_weapon;
  } else {
    /* Just remove the slot */
    inventory_remove_slot(&p->inventory, slot);
  }

  /* Equip new weapon */
  p->equipped_weapon = new_weapon;
  p->has_weapon = 1;

  return RESULT_SUCCESS;
}

ResultCode player_use_item(Player *p, int slot) {
  if (!p)
    return RESULT_FAIL;

  InventorySlot *inv_slot = inventory_get_slot(&p->inventory, slot);
  if (!inv_slot || inv_slot->type != SLOT_ITEM) {
    return RESULT_INVALID_SLOT;
  }

  Item *item = &inv_slot->data.item;

  if (!item_is_consumable(item)) {
    return RESULT_FAIL; /* Can't use non-consumables */
  }

  /* Apply effect based on consumable type */
  switch (item->consumable_type) {
  case CONSUMABLE_HEAL:
    player_heal(p, item->value);
    printf("Used %s! Restored %d HP.\n", item->name, item->value);
    break;

  case CONSUMABLE_BUFF_ATK:
    player_add_buff(p, CONSUMABLE_BUFF_ATK, item->value, item->duration);
    printf("Used %s! ATK +%d%% for %d turns.\n", item->name, item->value,
           item->duration);
    break;

  case CONSUMABLE_BUFF_DEF:
    player_add_buff(p, CONSUMABLE_BUFF_DEF, item->value, item->duration);
    printf("Used %s! DEF +%d%% for %d turns.\n", item->name, item->value,
           item->duration);
    break;

  case CONSUMABLE_FULL_RESTORE:
    player_full_restore(p);
    printf("Used %s! Fully restored!\n", item->name);
    break;

  case CONSUMABLE_CURE_POISON:
    printf("Used %s! Status cured.\n", item->name);
    break;

  default:
    return RESULT_FAIL;
  }

  /* Consume the item */
  inventory_use_item_at(&p->inventory, slot);

  return RESULT_SUCCESS;
}

void player_add_buff(Player *p, ConsumableType type, int value, int duration) {
  if (!p || duration <= 0)
    return;

  /* Check if same buff exists - refresh it */
  for (int i = 0; i < p->buff_count; i++) {
    if (p->active_buffs[i].type == type) {
      /* Refresh with stronger/longer effect */
      if (value > p->active_buffs[i].value) {
        p->active_buffs[i].value = value;
      }
      if (duration > p->active_buffs[i].turns_remaining) {
        p->active_buffs[i].turns_remaining = duration;
      }
      return;
    }
  }

  /* Add new buff if space available */
  if (p->buff_count < BUFF_MAX_ACTIVE) {
    p->active_buffs[p->buff_count].type = type;
    p->active_buffs[p->buff_count].value = value;
    p->active_buffs[p->buff_count].turns_remaining = duration;
    p->buff_count++;
  }
}

void player_tick_buffs(Player *p) {
  if (!p)
    return;

  for (int i = 0; i < p->buff_count; i++) {
    p->active_buffs[i].turns_remaining--;

    if (p->active_buffs[i].turns_remaining <= 0) {
      /* Remove expired buff by shifting remaining buffs */
      for (int j = i; j < p->buff_count - 1; j++) {
        p->active_buffs[j] = p->active_buffs[j + 1];
      }
      p->buff_count--;
      i--; /* Check this index again */
    }
  }
}

int player_has_buff(const Player *p, ConsumableType type) {
  if (!p)
    return 0;

  for (int i = 0; i < p->buff_count; i++) {
    if (p->active_buffs[i].type == type) {
      return 1;
    }
  }
  return 0;
}

int player_take_damage(Player *p, int amount) {
  if (!p)
    return 0;

  int defense = player_get_defense(p);
  int reduced = amount - (int)(defense * DEF_REDUCTION_FACTOR);
  if (reduced < 1) {
    reduced = 1;
  }

  p->current_hp -= reduced;

  if (p->current_hp <= 0) {
    p->current_hp = 0;
    return 1;
  }

  return 0;
}

void player_heal(Player *p, int amount) {
  if (!p)
    return;

  p->current_hp += amount;
  if (p->current_hp > p->max_hp) {
    p->current_hp = p->max_hp;
  }
}

void player_full_restore(Player *p) {
  if (!p)
    return;

  p->current_hp = p->max_hp;
  /* Clear all debuffs (if we add those later) */
}

int player_is_alive(const Player *p) {
  if (!p)
    return 0;
  return p->current_hp > 0;
}

void player_add_gold(Player *p, int amount) {
  if (!p || amount <= 0)
    return;
  p->gold += amount;
}

ResultCode player_spend_gold(Player *p, int amount) {
  if (!p)
    return RESULT_FAIL;
  if (p->gold < amount)
    return RESULT_NOT_ENOUGH_GOLD;

  p->gold -= amount;
  return RESULT_SUCCESS;
}

void player_print_status(const Player *p) {
  if (!p)
    return;

  printf("[%s] HP: %d/%d | ATK: %d | DEF: %d | Gold: %d", p->name,
         p->current_hp, p->max_hp, player_get_attack(p), player_get_defense(p),
         p->gold);

  if (p->has_weapon && strcmp(p->equipped_weapon.id, "fists") != 0) {
    printf(" | ");
    weapon_print(&p->equipped_weapon);
  }
  printf("\n");

  if (p->buff_count > 0) {
    player_print_buffs(p);
  }
}

void player_print_detailed(const Player *p) {
  if (!p)
    return;

  printf("\n========== %s ==========\n", p->name);
  printf("HP: %d/%d\n", p->current_hp, p->max_hp);
  printf("Base ATK: %d\n", p->base_attack);
  printf("Base DEF: %d\n", p->base_defense);
  printf("Total ATK: %d\n", player_get_attack(p));
  printf("Total DEF: %d\n", player_get_defense(p));
  printf("Gold: %d\n", p->gold);
  printf("\nEquipped: ");
  if (p->has_weapon) {
    weapon_print(&p->equipped_weapon);
  } else {
    printf("None");
  }
  printf("\n");

  if (p->buff_count > 0) {
    printf("\nActive Buffs:\n");
    player_print_buffs(p);
  }

  printf("===========================\n");
}

void player_print_buffs(const Player *p) {
  if (!p || p->buff_count == 0)
    return;

  printf("  Buffs: ");
  for (int i = 0; i < p->buff_count; i++) {
    if (p->active_buffs[i].type == CONSUMABLE_BUFF_ATK) {
      printf("[ATK+%d%% %dt]", p->active_buffs[i].value,
             p->active_buffs[i].turns_remaining);
    } else if (p->active_buffs[i].type == CONSUMABLE_BUFF_DEF) {
      printf("[DEF+%d%% %dt]", p->active_buffs[i].value,
             p->active_buffs[i].turns_remaining);
    }
    if (i < p->buff_count - 1) {
      printf(" ");
    }
  }
  printf("\n");
}
