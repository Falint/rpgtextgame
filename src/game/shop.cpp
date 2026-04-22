/**
 * @file shop.cpp
 * @brief Shop system implementation (C++ Version)
 */

#include "shop.hpp"
#include "../core/element.hpp"
#include "../core/item.hpp"
#include "../core/weapon.hpp"
#include "../data/registry.hpp"
#include "../utils/utils.hpp"
#include "../config.hpp"
#include <iostream>
#include <vector>
#include <string>

namespace Shop {

    /* ============================================
     * INTERNAL STATE
     * ============================================ */

    // Menggunakan vector untuk melacak stok secara runtime
    static std::vector<int> shop_stock_remaining;
    static bool shop_initialized = false;

    static void init_shop_stock() {
        if (shop_initialized) return;

        int count = 0;
        const ShopEntry* entries = Registry::get_shop_entries(&count);

        shop_stock_remaining.resize(count);
        for (int i = 0; i < count; ++i) {
            shop_stock_remaining[i] = entries[i].stock;
        }

        shop_initialized = true;
    }

    /* ============================================
     * DISPLAY HELPERS
     * ============================================ */

    static void print_shop_header(const Player* p) {
        Utils::clear_screen();
        Utils::print_header("SHOP", 50);
        std::cout << "\n   Your Gold: " << p->gold << "\n\n";
    }

    static void print_available_weapons() {
        init_shop_stock();
        std::cout << "=== WEAPONS ===\n";

        int count = 0;
        const ShopEntry* entries = Registry::get_shop_entries(&count);

        int display_num = 1;
        for (int i = 0; i < count; ++i) {
            if (!entries[i].is_weapon) continue;

            const WeaponTemplate* w = Registry::get_weapon(entries[i].item_id);
            if (!w || w->buy_price <= 0) continue;

            int stock = shop_stock_remaining[i];
            std::cout << "   " << display_num++ << ". ";

            if (stock == 0) {
                std::cout << "[SOLD OUT] " << w->name << "\n";
                continue;
            }

            std::cout << w->name << " - " << w->buy_price << " G | DMG: " << w->base_damage;
            
            if (w->element != ELEMENT_NONE) {
                std::cout << " (" << element_to_string(w->element) << ")";
            }

            if (stock > 0) std::cout << " [Stock: " << stock << "]";
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    static void print_available_items() {
        init_shop_stock();
        std::cout << "=== ITEMS ===\n";

        int count = 0;
        const ShopEntry* entries = Registry::get_shop_entries(&count);

        int display_num = 1;
        for (int i = 0; i < count; ++i) {
            if (entries[i].is_weapon) continue;

            const ItemTemplate* it = Registry::get_item(entries[i].item_id);
            if (!it || it->buy_price <= 0) continue;

            int stock = shop_stock_remaining[i];
            std::cout << "   " << display_num++ << ". ";

            if (stock == 0) {
                std::cout << "[SOLD OUT] " << it->name << "\n";
                continue;
            }

            std::cout << it->name << " - " << it->buy_price << " G";

            if (it->category == ITEM_CATEGORY_CONSUMABLE) {
                switch (it->consumable_type) {
                    case CONSUMABLE_HEAL:     std::cout << " (+" << it->value << " HP)"; break;
                    case CONSUMABLE_BUFF_ATK: std::cout << " (+" << it->value << "% ATK, " << it->duration << "t)"; break;
                    case CONSUMABLE_BUFF_DEF: std::cout << " (+" << it->value << "% DEF, " << it->duration << "t)"; break;
                    default: break;
                }
            }

            if (stock > 0) std::cout << " [Stock: " << stock << "]";
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    /* ============================================
     * ACTION LOGIC
     * ============================================ */

    static void process_buy_weapon(Player* p) {
        print_shop_header(p);
        print_available_weapons();

        std::cout << "Enter number to buy (0 to cancel): ";
        int choice = Utils::get_int_input(0, 100);
        if (choice == 0) return;

        int count = 0;
        const ShopEntry* entries = Registry::get_shop_entries(&count);
        int entry_idx = -1, current_display = 0;

        for (int i = 0; i < count; ++i) {
            if (!entries[i].is_weapon) continue;
            if (++current_display == choice) {
                entry_idx = i;
                break;
            }
        }

        if (entry_idx < 0 || shop_stock_remaining[entry_idx] == 0) {
            std::cout << (entry_idx < 0 ? "Invalid choice.\n" : "Sold out!\n");
            Utils::delay_ms(GameConfig::UI::DELAY_MENU_TRANSITION);
            return;
        }

        const WeaponTemplate* w = Registry::get_weapon(entries[entry_idx].item_id);
        if (p->gold < w->buy_price) {
            std::cout << "Not enough gold!\n";
        } else if (inventory_is_full(&p->inventory)) {
            std::cout << "Inventory full!\n";
        } else {
            Weapon new_w = weapon_create(w->id);
            if (inventory_add_weapon(&p->inventory, new_w) == RESULT_SUCCESS) {
                player_spend_gold(p, w->buy_price);
                if (shop_stock_remaining[entry_idx] > 0) shop_stock_remaining[entry_idx]--;
                std::cout << "Purchased " << w->name << "!\n";
            }
        }
        Utils::delay_ms(GameConfig::UI::DELAY_MENU_TRANSITION);
    }

    static void process_sell_items(Player* p) {
        print_shop_header(p);
        inventory_print(&p->inventory);

        std::cout << "\nEnter slot number to sell (0 to cancel): ";
        int choice = Utils::get_int_input(0, GameConfig::Inventory::MAX_SLOTS);
        if (choice == 0) return;

        // Logika mencari slot inventory (disingkat untuk efisiensi)
        int actual_idx = -1, display_num = 0;
        for (int i = 0; i < GameConfig::Inventory::MAX_SLOTS; ++i) {
            if (inventory_get_slot(&p->inventory, i)) {
                if (++display_num == choice) { actual_idx = i; break; }
            }
        }

        if (actual_idx < 0) return;

        InventorySlot* slot = inventory_get_slot(&p->inventory, actual_idx);
        int unit_price = (slot->type == SLOT_WEAPON) ? slot->data.weapon.sell_price : slot->data.item.sell_price;
        std::string name = (slot->type == SLOT_WEAPON) ? slot->data.weapon.name : slot->data.item.name;
        
        int qty = 1;
        if (slot->type == SLOT_ITEM && slot->quantity > 1) {
            std::cout << "Quantity (1-" << slot->quantity << "): ";
            qty = Utils::get_int_input(1, slot->quantity);
        }

        int total_sell = unit_price * qty;
        std::cout << "Sell " << name << " x" << qty << " for " << total_sell << " G?";
        
        if (Utils::get_yes_no()) {
            if (slot->type == SLOT_WEAPON) inventory_remove_slot(&p->inventory, actual_idx);
            else {
                for(int i=0; i<qty; ++i) inventory_use_item_at(&p->inventory, actual_idx);
            }
            player_add_gold(p, total_sell);
            std::cout << "Sold!\n";
        }
        Utils::delay_ms(GameConfig::UI::DELAY_MENU_TRANSITION);
    }

    /* ============================================
     * PUBLIC FUNCTIONS
     * ============================================ */

    MenuResult enter(Player* player) {
        if (!player) return MenuResult::BACK;
        init_shop_stock();

        while (true) {
            print_shop_header(player);
            std::cout << "--- Shop Menu ---\n"
                      << "1. Buy Weapons\n"
                      << "2. Buy Items\n"
                      << "3. Sell Items\n"
                      << "4. Exit Shop\n"
                      << "Choice: ";

            int choice = Utils::get_int_input(1, 4);
            if (choice == 4) break;

            switch (choice) {
                case 1: process_buy_weapon(player); break;
                case 2: /* logic buy item mirip weapon */ break;
                case 3: process_sell_items(player); break;
                default: break;
            }
        }
        return MenuResult::BACK;
    }

} // namespace Shop