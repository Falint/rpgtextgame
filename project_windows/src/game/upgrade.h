/**
 * @file upgrade.h
 * @brief Weapon upgrade system
 */

#ifndef UPGRADE_H
#define UPGRADE_H

#include "../core/player.h"
#include "../types.h"

/**
 * @brief Enter the upgrade menu
 */
MenuResult upgrade_enter(Player *player);

/**
 * @brief Check if player can upgrade a weapon
 */
int upgrade_can_upgrade_weapon(const Player *player, int weapon_slot);

/**
 * @brief Perform weapon upgrade
 */
ResultCode upgrade_weapon(Player *player, int weapon_slot);

/**
 * @brief Print upgrade requirements for a weapon
 */
void upgrade_print_requirements(const Weapon *weapon, const Player *player);

#endif /* UPGRADE_H */
