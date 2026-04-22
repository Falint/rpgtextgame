/**
 * @file items.h
 * @brief Item database declarations
 */

#ifndef ITEMS_H
#define ITEMS_H

#include "registry.h"

/**
 * @brief Get the item database array
 * @param out_count Output: number of items
 * @return Pointer to item template array
 */
const ItemTemplate *items_get_database(int *out_count);

#endif /* ITEMS_H */
