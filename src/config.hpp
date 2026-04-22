/**
 * @file config.hpp
 * @brief Central configuration for TextRPG game (C++ Version)
 *
 * Menggunakan constexpr dan namespace untuk type-safety dan pengorganisasian yang lebih baik.
 */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <cstdint>
#include <string_view>

namespace GameConfig {

    /* ============================================
     * STRING LIMITS
     * ============================================ */
    namespace Limits {
        constexpr std::size_t MAX_NAME_LEN = 32;
        constexpr std::size_t MAX_DESC_LEN = 128;
        constexpr std::size_t MAX_ID_LEN   = 32;
    }

    /* ============================================
     * PLAYER DEFAULTS
     * ============================================ */
    namespace Player {
        constexpr int32_t STARTING_HP    = 100;
        constexpr int32_t STARTING_GOLD  = 300;
        constexpr int32_t STARTING_DEF   = 10;
    }

    /* ============================================
     * INVENTORY
     * ============================================ */
    namespace Inventory {
        constexpr int32_t MAX_SLOTS      = 20;
        constexpr int32_t STACK_LIMIT    = 99;
    }

    /* ============================================
     * COMBAT SYSTEM
     * ============================================ */
    namespace Combat {
        // Element damage modifiers
        constexpr float DAMAGE_MOD_STRONG  = 2.0f;
        constexpr float DAMAGE_MOD_WEAK    = 0.5f;
        constexpr float DAMAGE_MOD_NEUTRAL = 1.0f;

        // Critical hits
        constexpr float CRIT_CHANCE_BASE   = 0.1f;
        constexpr float CRIT_DAMAGE_MULT   = 1.5f;

        // Defense calculation: damage = attack - (defense * DEF_REDUCTION)
        constexpr float DEF_REDUCTION_FACTOR = 0.5f;
    }

    /* ============================================
     * LOOT SYSTEM
     * ============================================ */
    namespace Loot {
        constexpr float   DROP_CHANCE_BASE = 0.3f;
        constexpr int32_t GOLD_DROP_MIN    = 10;
        constexpr int32_t GOLD_DROP_MAX    = 50;
        constexpr int32_t MAX_ENTRIES      = 5;
    }

    /* ============================================
     * UI DELAYS (milliseconds)
     * Menggunakan int agar kompatibel dengan library seperti <thread> atau SDL
     * ============================================ */
    namespace UI {
        constexpr int DELAY_BATTLE_START   = 1000;
        constexpr int DELAY_TURN_CHANGE    = 800;
        constexpr int DELAY_AFTER_ATTACK   = 500;
        constexpr int DELAY_BATTLE_END     = 1200;
        constexpr int DELAY_MENU_TRANSITION = 300;
        constexpr int DELAY_LOOT_DROP      = 400;
    }

    /* ============================================
     * REGISTRY LIMITS
     * ============================================ */
    namespace Registry {
        constexpr int32_t MAX_MONSTERS        = 50;
        constexpr int32_t MAX_WEAPONS         = 50;
        constexpr int32_t MAX_ITEMS           = 100;
        constexpr int32_t MAX_UPGRADE_RECIPES = 50;
        constexpr int32_t MAX_SHOP_ITEMS      = 20;
    }

    /* ============================================
     * WEAPON UPGRADE SYSTEM
     * ============================================ */
    namespace Upgrades {
        constexpr int32_t WEAPON_MAX_TIER        = 5;
        constexpr float   WEAPON_TIER_DAMAGE_MULT = 1.15f; 
    }

    /* ============================================
     * BUFF SYSTEM
     * ============================================ */
    namespace Buffs {
        constexpr int32_t MAX_ACTIVE       = 3;
        constexpr int32_t ATK_PERCENTAGE   = 25;
        constexpr int32_t DEF_PERCENTAGE   = 25;
    }

    /* ============================================
     * SHOP SYSTEM
     * ============================================ */
    namespace Shop {
        constexpr float SELL_RATIO = 0.5f;
    }

} // namespace GameConfig

#endif // CONFIG_HPP