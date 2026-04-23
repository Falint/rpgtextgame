/**
 * @file shop.h
 * @brief Shop system
 */

#ifndef SHOP_H
#define SHOP_H

#include "../core/player.h"
#include "../types.h"

/**
 * @brief Enter the shop
 * @return MENU_BACK when done shopping
 */
MenuResult shop_enter(Player *player);

#endif /* SHOP_H */
