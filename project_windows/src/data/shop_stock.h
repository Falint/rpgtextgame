/**
 * @file shop_stock.h
 * @brief Shop inventory declarations
 */

#ifndef SHOP_STOCK_H
#define SHOP_STOCK_H

#include "registry.h"

/**
 * @brief Get the shop stock array
 * @param out_count Output: number of shop entries
 * @return Pointer to shop entry array
 */
const ShopEntry *shop_stock_get_database(int *out_count);

#endif /* SHOP_STOCK_H */
