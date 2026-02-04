/**
 * @file loot.h
 * @brief Loot drop system
 */

#ifndef LOOT_H
#define LOOT_H

#include "../types.h"
#include "enemy.h"
#include "item.h"
#include "player.h"

/**
 * @brief Generated loot from defeated enemy
 */
struct LootDrop {
  int gold;
  Item items[MAX_LOOT_ENTRIES];
  int item_count;
};

/**
 * @brief Generate loot from defeated enemy
 */
LootDrop loot_generate(const Enemy *enemy);

/**
 * @brief Add loot to player's inventory and gold
 */
void loot_collect(Player *player, const LootDrop *loot);

/**
 * @brief Print loot drop summary
 */
void loot_print(const LootDrop *loot);

#endif /* LOOT_H */
