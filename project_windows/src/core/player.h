/**
 * @file player.h
 * @brief Player system with buffs and equipment
 */

#ifndef PLAYER_H
#define PLAYER_H

#include "../types.h"
#include "inventory.h"
#include "weapon.h"

/**
 * @brief Active buff on player
 */
struct Buff {
  ConsumableType type;
  int value; /* Effect strength (percentage) */
  int turns_remaining;
};

/**
 * @brief Player state
 */
struct Player {
  char name[MAX_NAME_LEN];
  int max_hp;
  int current_hp;
  int base_attack;
  int base_defense;
  int gold;

  Inventory inventory;
  Weapon equipped_weapon;
  int has_weapon;

  /* Active buffs */
  Buff active_buffs[BUFF_MAX_ACTIVE];
  int buff_count;
};

/**
 * @brief Initialize new player
 */
void player_init(Player *p, const char *name);

/**
 * @brief Get player's total attack (base + weapon + buffs)
 */
int player_get_attack(const Player *p);

/**
 * @brief Get player's total defense (base + buffs)
 */
int player_get_defense(const Player *p);

/**
 * @brief Equip weapon from inventory
 * @param slot Inventory slot index
 */
ResultCode player_equip_weapon(Player *p, int slot);

/**
 * @brief Use consumable item from inventory
 */
ResultCode player_use_item(Player *p, int slot);

/**
 * @brief Apply buff to player
 */
void player_add_buff(Player *p, ConsumableType type, int value, int duration);

/**
 * @brief Tick down buff durations (call at end of turn)
 */
void player_tick_buffs(Player *p);

/**
 * @brief Check if player has specific buff type
 */
int player_has_buff(const Player *p, ConsumableType type);

/**
 * @brief Apply damage to player (considers defense)
 * @return 1 if player died, 0 otherwise
 */
int player_take_damage(Player *p, int amount);

/**
 * @brief Heal player
 */
void player_heal(Player *p, int amount);

/**
 * @brief Full heal and cure all status
 */
void player_full_restore(Player *p);

/**
 * @brief Check if player is alive
 */
int player_is_alive(const Player *p);

/**
 * @brief Add gold to player
 */
void player_add_gold(Player *p, int amount);

/**
 * @brief Spend gold
 */
ResultCode player_spend_gold(Player *p, int amount);

/**
 * @brief Print player status
 */
void player_print_status(const Player *p);

/**
 * @brief Print detailed player info
 */
void player_print_detailed(const Player *p);

/**
 * @brief Print active buffs
 */
void player_print_buffs(const Player *p);

#endif /* PLAYER_H */
