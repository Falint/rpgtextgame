/**
 * @file enemy.c
 * @brief Enemy system implementation
 */

#include "enemy.h"
#include "../utils/utils.h"
#include "element.h"
#include <stdio.h>

Enemy enemy_create(const char *template_id) {
  Enemy e = {0};

  const MonsterTemplate *tmpl = registry_get_monster(template_id);
  if (!tmpl) {
    /* Return a default slime */
    tmpl = registry_get_monster("slime");
    if (!tmpl) {
      /* Fallback if registry not initialized */
      safe_strcpy(e.id, "slime", MAX_ID_LEN);
      safe_strcpy(e.name, "Slime", MAX_NAME_LEN);
      e.max_hp = 30;
      e.current_hp = 30;
      e.attack = 8;
      e.defense = 2;
      e.element = ELEMENT_WATER;
      e.type = MONSTER_TYPE_NORMAL;
      e.gold_min = 10;
      e.gold_max = 20;
      e.template = NULL;
      return e;
    }
  }

  return enemy_create_from_template(tmpl);
}

Enemy enemy_create_from_template(const MonsterTemplate *tmpl) {
  Enemy e = {0};

  if (!tmpl) {
    return enemy_create("slime");
  }

  safe_strcpy(e.id, tmpl->id, MAX_ID_LEN);
  safe_strcpy(e.name, tmpl->name, MAX_NAME_LEN);
  e.max_hp = tmpl->base_hp;
  e.current_hp = tmpl->base_hp;
  e.attack = tmpl->base_attack;
  e.defense = tmpl->base_defense;
  e.element = tmpl->element;
  e.type = tmpl->type;
  e.gold_min = tmpl->gold_min;
  e.gold_max = tmpl->gold_max;
  e.template = tmpl;

  return e;
}

Enemy enemy_create_random(MonsterType type) {
  const MonsterTemplate *monsters[MAX_MONSTERS];
  int count = registry_get_monsters_by_type(type, monsters, MAX_MONSTERS);

  if (count == 0) {
    /* Fallback to any monster */
    count = registry_get_monster_count();
    if (count == 0) {
      return enemy_create("slime");
    }
    int index = random_range(0, count - 1);
    return enemy_create_from_template(registry_get_monster_by_index(index));
  }

  int index = random_range(0, count - 1);
  return enemy_create_from_template(monsters[index]);
}

int enemy_take_damage(Enemy *e, int amount) {
  if (!e)
    return 0;

  /* Apply defense reduction */
  int reduced = amount - (int)(e->defense * DEF_REDUCTION_FACTOR);
  if (reduced < 1) {
    reduced = 1; /* Minimum 1 damage */
  }

  e->current_hp -= reduced;

  if (e->current_hp <= 0) {
    e->current_hp = 0;
    return 1; /* Died */
  }

  return 0;
}

int enemy_calculate_damage(const Enemy *e) {
  if (!e)
    return 0;
  return e->attack;
}

int enemy_roll_gold(const Enemy *e) {
  if (!e)
    return 0;
  return random_range(e->gold_min, e->gold_max);
}

int enemy_is_alive(const Enemy *e) {
  if (!e)
    return 0;
  return e->current_hp > 0;
}

const char *monster_type_to_string(MonsterType type) {
  switch (type) {
  case MONSTER_TYPE_NORMAL:
    return "Normal";
  case MONSTER_TYPE_ELITE:
    return "Elite";
  case MONSTER_TYPE_BOSS:
    return "Boss";
  default:
    return "Unknown";
  }
}

void enemy_print_status(const Enemy *e) {
  if (!e)
    return;

  /* Show type indicator for non-normal monsters */
  if (e->type == MONSTER_TYPE_ELITE) {
    printf("[ELITE] ");
  } else if (e->type == MONSTER_TYPE_BOSS) {
    printf("[BOSS] ");
  }

  printf("%s - HP: %d/%d | ATK: %d | DEF: %d", e->name, e->current_hp,
         e->max_hp, e->attack, e->defense);

  if (e->element != ELEMENT_NONE) {
    printf(" | %s", element_to_string(e->element));
  }
  printf("\n");
}

void enemy_print_detailed(const Enemy *e) {
  if (!e)
    return;

  printf("=== %s ===\n", e->name);
  printf("Type: %s\n", monster_type_to_string(e->type));
  printf("HP: %d/%d\n", e->current_hp, e->max_hp);
  printf("Attack: %d\n", e->attack);
  printf("Defense: %d\n", e->defense);
  printf("Element: %s\n", element_to_string(e->element));
  printf("Gold Drop: %d - %d\n", e->gold_min, e->gold_max);
}
