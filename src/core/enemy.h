/**
 * @file enemy.h
 * @brief Enemy system with monster templates
 */

#ifndef ENEMY_H
#define ENEMY_H

#include "../data/registry.h"
#include "../types.h"

/**
 * @brief Runtime enemy instance
 */
struct Enemy {
  char id[MAX_ID_LEN];
  char name[MAX_NAME_LEN];
  int max_hp;
  int current_hp;
  int attack;
  int defense;
  Element element;
  MonsterType type;
  int gold_min;
  int gold_max;
  const MonsterTemplate *template; /* Reference for loot table */
};

/**
 * @brief Create enemy from template ID
 */
Enemy enemy_create(const char *template_id);

/**
 * @brief Create enemy from template
 */
Enemy enemy_create_from_template(const MonsterTemplate *tmpl);

/**
 * @brief Create random enemy of specified type
 */
Enemy enemy_create_random(MonsterType type);

/**
 * @brief Apply damage to enemy
 * @return 1 if enemy died, 0 otherwise
 */
int enemy_take_damage(Enemy *e, int amount);

/**
 * @brief Calculate enemy's attack damage
 */
int enemy_calculate_damage(const Enemy *e);

/**
 * @brief Roll gold drop amount
 */
int enemy_roll_gold(const Enemy *e);

/**
 * @brief Check if enemy is alive
 */
int enemy_is_alive(const Enemy *e);

/**
 * @brief Get monster type as string
 */
const char *monster_type_to_string(MonsterType type);

/**
 * @brief Print enemy status (single line)
 */
void enemy_print_status(const Enemy *e);

/**
 * @brief Print detailed enemy info
 */
void enemy_print_detailed(const Enemy *e);

#endif /* ENEMY_H */
