/**
 * @file shop_stock.cpp
 * @brief Shop Inventory Database Implementation (C++ Version)
 */

#include "shop_stock.hpp"

struct ShopEntry {
    std::string item_id;
    bool is_weapon;
    int32_t stock;
};

namespace ShopDatabase {

    /**
     * Database stok toko menggunakan initializer list.
     * Struktur: {item_id, is_weapon, stock}
     */
    static const ShopEntry SHOP_DB[] = {
        /* =========================================
         * WEAPONS FOR SALE
         * ========================================= */
        {"iron_sword",      true, -1},
        {"steel_sword",     true, -1},
        {"fire_blade",      true,  1},
        {"aqua_saber",      true,  1},
        {"earth_cleaver",   true,  1},
        {"wind_cutter",     true,  1},
        {"iron_axe",        true, -1},
        {"wooden_staff",    true, -1},
        {"fire_staff",      true,  1},
        {"ice_staff",       true,  1},
        {"iron_dagger",     true, -1},
        {"assassin_blade",  true,  1},
        {"hunting_bow",     true, -1},
        {"gale_bow",        true,  1},

        /* =========================================
         * CONSUMABLES FOR SALE
         * ========================================= */
        {"small_potion",    false, -1},
        {"medium_potion",   false, -1},
        {"large_potion",    false, -1},
        {"full_restore",    false,  5},
        {"atk_elixir",      false, -1},
        {"def_elixir",      false, -1},
        {"power_surge",     false,  3},
        {"iron_skin",       false,  3},
        {"antidote",        false, -1}
    };

    static const int32_t SHOP_DB_COUNT = sizeof(SHOP_DB) / sizeof(SHOP_DB[0]);

    const ShopEntry* get_database(int32_t* out_count) {
        if (out_count) {
            *out_count = SHOP_DB_COUNT;
        }
        return SHOP_DB;
    }

    std::vector<const ShopEntry*> get_all_entries() {
        std::vector<const ShopEntry*> entries;
        entries.reserve(SHOP_DB_COUNT); // Optimasi alokasi memori
        for (int i = 0; i < SHOP_DB_COUNT; ++i) {
            entries.push_back(&SHOP_DB[i]);
        }
        return entries;
    }

} // namespace ShopDatabase