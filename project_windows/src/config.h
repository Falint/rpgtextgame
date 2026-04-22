/**
 * @file config.h
 * @brief Central configuration for TextRPG game
 *
 * All game constants are defined here to avoid magic numbers.
 * Modify these values to tune game balance.
 */

#ifndef CONFIG_H
#define CONFIG_H

/* ============================================
 * STRING LIMITS
 * ============================================ */
#define MAX_NAME_LEN        32
#define MAX_DESC_LEN        128
#define MAX_ID_LEN          32

/* ============================================
 * PLAYER DEFAULTS
 * ============================================ */
#define PLAYER_STARTING_HP      100
#define PLAYER_STARTING_GOLD    300
#define PLAYER_STARTING_DEF     10

/* ============================================
 * INVENTORY
 * ============================================ */
#define INVENTORY_MAX_SLOTS     20
#define INVENTORY_STACK_LIMIT   99

/* ============================================
 * COMBAT SYSTEM
 * ============================================ */
/* Element damage modifiers */
#define DAMAGE_MOD_STRONG       2.0f
#define DAMAGE_MOD_WEAK         0.5f
#define DAMAGE_MOD_NEUTRAL      1.0f

/* Critical hits */
#define CRIT_CHANCE_BASE        0.1f
#define CRIT_DAMAGE_MULT        1.5f

/* Defense calculation: damage = attack - (defense * DEF_REDUCTION) */
#define DEF_REDUCTION_FACTOR    0.5f

/* ============================================
 * LOOT SYSTEM
 * ============================================ */
#define LOOT_DROP_CHANCE_BASE   0.3f
#define GOLD_DROP_MIN           10
#define GOLD_DROP_MAX           50
#define MAX_LOOT_ENTRIES        5

/* ============================================
 * UI DELAYS (milliseconds)
 * For dramatic effect during gameplay
 * ============================================ */
#define DELAY_BATTLE_START      1000
#define DELAY_TURN_CHANGE       800
#define DELAY_AFTER_ATTACK      500
#define DELAY_BATTLE_END        1200
#define DELAY_MENU_TRANSITION   300
#define DELAY_LOOT_DROP         400

/* ============================================
 * REGISTRY LIMITS
 * Maximum number of each entity type
 * ============================================ */
#define MAX_MONSTERS            50
#define MAX_WEAPONS             50
#define MAX_ITEMS               100
#define MAX_UPGRADE_RECIPES     50
#define MAX_SHOP_ITEMS          20

/* ============================================
 * WEAPON UPGRADE SYSTEM
 * ============================================ */
#define WEAPON_MAX_TIER         5
#define WEAPON_TIER_DAMAGE_MULT 1.15f  /* Each tier adds 15% base damage */

/* ============================================
 * BUFF SYSTEM
 * ============================================ */
#define BUFF_MAX_ACTIVE         3      /* Max concurrent buffs */
#define BUFF_ATK_PERCENTAGE     25     /* Default ATK buff % */
#define BUFF_DEF_PERCENTAGE     25     /* Default DEF buff % */

/* ============================================
 * SHOP SYSTEM
 * ============================================ */
#define SHOP_SELL_RATIO         0.5f   /* Sell price = buy price * ratio */

#endif /* CONFIG_H */
