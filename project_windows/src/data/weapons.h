/**
 * @file weapons.h
 * @brief Weapon database declarations
 */

#ifndef WEAPONS_H
#define WEAPONS_H

#include "registry.h"

/**
 * @brief Get the weapon database array
 * @param out_count Output: number of weapons
 * @return Pointer to weapon template array
 */
const WeaponTemplate *weapons_get_database(int *out_count);

#endif /* WEAPONS_H */
