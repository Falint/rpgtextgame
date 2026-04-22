/**
 * @file enemy.cpp
 * @brief Enemy system implementation
 */

#include "enemy.h"
#include "../utils/utils.h"
#include "element.h"
#include <iostream>
#include <cstring>

Enemy enemy_create(const char* template_id) {
  Enemy e{}; /* C++ uniform initialization (zero-init for ints, empty for strings) */

  const MonsterTemplate* tmpl = registry_get_monster(template_id);
  if (!tmpl) {
    /* Return a default slime */
    tmpl = registry_get_monster("slime");
    if (!tmpl) {
      /* Fallback if registry not initialized */
      return {
        "slime",
        "Slime",
        30,
        30,
        8,
        2,
        Element::WATER,
        MonsterType::NORMAL,
        10,
        20,
        nullptr
      };
    }
  }

  return enemy_create_from_template(tmpl);
}

Enemy enemy_create_from_template(const MonsterTemplate* tmpl) {
  if (!tmpl) {
    return enemy_create("slime");
  }

  Enemy e{};
  std::strncpy(e.id, tmpl->id, sizeof(e.id) - 1);
  e.id[sizeof(e.id) - 1] = '\0';
  std::strncpy(e.name, tmpl->name, sizeof(e.name) - 1);
  e.name[sizeof(e.name) - 1] = '\0';
  e.current_hp = tmpl->base_hp;
  e.max_hp = tmpl->base_hp;
  e.attack = tmpl->base_attack;
  e.defense = tmpl->base_defense;
  e.element = tmpl->element;
  e.type = tmpl->type;
  e.gold_min = tmpl->gold_min;
  e.gold_max = tmpl->gold_max;
  return e;
}

Enemy enemy_create_random(MonsterType type) {
  const MonsterTemplate* monsters[MAX_MONSTERS];
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

bool enemy_take_damage(Enemy* e, int amount) {
  if (!e) {
    return false;
  }

  /* Apply defense reduction */
  int reduced = amount - static_cast<int>(e->defense * DEF_REDUCTION_FACTOR);
  if (reduced < 1) {
    reduced = 1; /* Minimum 1 damage */
  }

  e->current_hp -= reduced;

  if (e->current_hp <= 0) {
    e->current_hp = 0;
    return true; /* Died */
  }

  return false;
}

int enemy_calculate_damage(const Enemy* e) {
  if (!e) {
    return 0;
  }
  return e->attack;
}

int enemy_roll_gold(const Enemy* e) {
  if (!e) {
    return 0;
  }
  return random_range(e->gold_min, e->gold_max);
}

bool enemy_is_alive(const Enemy* e) {
  if (!e) {
    return false;
  }
  return e->current_hp > 0;
}

const char* monster_type_to_string(MonsterType type) {
  switch (type) {
    case MonsterType::NORMAL: return "Normal";
    case MonsterType::ELITE:  return "Elite";
    case MonsterType::BOSS:   return "Boss";
    default:                  return "Unknown";
  }
}

void enemy_print_status(const Enemy* e) {
  if (!e) {
    return;
  }

  /* Show type indicator for non-normal monsters */
  if (e->type == MonsterType::ELITE) {
    std::cout << "[ELITE] ";
  } else if (e->type == MonsterType::BOSS) {
    std::cout << "[BOSS] ";
  }

  std::cout << e->name << " - HP: " << e->current_hp << "/" << e->max_hp
            << " | ATK: " << e->attack << " | DEF: " << e->defense;

  if (e->element != Element::NONE) {
    std::cout << " | " << element_to_string(e->element);
  }
  std::cout << "\n";
}

void enemy_print_detailed(const Enemy* e) {
  if (!e) {
    return;
  }

  std::cout << "=== " << e->name << " ===\n"
            << "Type: " << monster_type_to_string(e->type) << "\n"
            << "HP: " << e->current_hp << "/" << e->max_hp << "\n"
            << "Attack: " << e->attack << "\n"
            << "Defense: " << e->defense << "\n"
            << "Element: " << element_to_string(e->element) << "\n"
            << "Gold Drop: " << e->gold_min << " - " << e->gold_max << "\n";
}