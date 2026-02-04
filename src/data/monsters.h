/**
 * @file monsters.h
 * @brief Monster database declarations
 */

#ifndef MONSTERS_H
#define MONSTERS_H

#include "registry.h"

/**
 * @brief Get the monster database array
 * @param out_count Output: number of monsters
 * @return Pointer to monster template array
 */
const MonsterTemplate *monsters_get_database(int *out_count);

#endif /* MONSTERS_H */
