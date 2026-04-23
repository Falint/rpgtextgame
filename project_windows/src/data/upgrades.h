/**
 * @file upgrades.h
 * @brief Upgrade recipes database declarations
 */

#ifndef UPGRADES_H
#define UPGRADES_H

#include "registry.h"

/**
 * @brief Get the upgrade recipes database array
 * @param out_count Output: number of recipes
 * @return Pointer to upgrade recipe array
 */
const UpgradeRecipe *upgrades_get_database(int *out_count);

#endif /* UPGRADES_H */
