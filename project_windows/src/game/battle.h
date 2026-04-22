/**
 * @file battle.h
 * @brief Battle system
 */

#ifndef BATTLE_H
#define BATTLE_H

#include "../core/enemy.h"
#include "../core/player.h"
#include "../types.h"

/**
 * @brief Start a battle between player and enemy
 * @return Battle result (WIN/LOSE/ESCAPE)
 */
BattleResult battle_start(Player *player, Enemy *enemy);

/**
 * @brief Start a random battle with specified monster type
 */
BattleResult battle_random(Player *player, MonsterType type);

#endif /* BATTLE_H */
