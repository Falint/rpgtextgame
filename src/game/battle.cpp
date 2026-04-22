/**
 * @file battle.cpp
 * @brief Battle system implementation (C++ Version)
 */

#include "battle.hpp"
#include "../core/element.hpp"
#include "../core/loot.hpp"
#include "../utils/utils.hpp"
#include "../config.hpp" // Menggunakan GameConfig yang kita buat sebelumnya
#include <iostream>
#include <algorithm>

namespace Battle {

    /* ============================================
     * INTERNAL HELPERS (Private to this file)
     * ============================================ */

    static void print_battle_header(const Player* p, const Enemy* e) {
        Utils::clear_screen();
        Utils::print_header("BATTLE!", 40);
        std::cout << "\n";
        player_print_status(p);
        std::cout << "\n";
        enemy_print_status(e);
        std::cout << "\n";
    }

    static void display_battle_menu() {
        std::cout << "\n--- Actions ---\n"
                  << "1. Attack\n"
                  << "2. Use Item\n"
                  << "3. Run\n"
                  << "Choice: ";
    }

    static int calculate_player_damage(const Player* p, const Enemy* e) {
        int base_damage = player_get_attack(p);

        /* Apply element modifier */
        Element weapon_element = ELEMENT_NONE;
        if (p->has_weapon) {
            weapon_element = p->equipped_weapon.element;
        }

        float modifier = element_get_modifier(weapon_element, e->element);
        int damage = static_cast<int>(base_damage * modifier);

        /* Apply enemy defense */
        damage -= static_cast<int>(e->defense * GameConfig::Combat::DEF_REDUCTION_FACTOR);
        
        return std::max(1, damage);
    }

    static void execute_player_attack(Player* p, Enemy* e) {
        int damage = calculate_player_damage(p, e);

        /* Check for critical hit */
        bool is_crit = Utils::random_chance(GameConfig::Combat::CRIT_CHANCE_BASE);
        if (is_crit) {
            damage = static_cast<int>(damage * GameConfig::Combat::CRIT_DAMAGE_MULT);
            std::cout << "\n>>> CRITICAL HIT! <<<\n";
        }

        /* Apply damage message */
        std::cout << "You attack " << e->name << " for " << damage << " damage!";

        /* Show element effectiveness */
        Element weapon_element = p->has_weapon ? p->equipped_weapon.element : ELEMENT_NONE;
        float mod = element_get_modifier(weapon_element, e->element);
        
        if (mod > 1.0f) {
            std::cout << " (Super Effective!)";
        } else if (mod < 1.0f) {
            std::cout << " (Not Very Effective...)";
        }
        std::cout << "\n";

        /* Apply damage to enemy current HP */
        e->current_hp -= damage;
        if (e->current_hp < 0) e->current_hp = 0;

        Utils::delay_ms(GameConfig::UI::DELAY_AFTER_ATTACK);
    }

    static void execute_enemy_attack(Enemy* e, Player* p) {
        std::cout << "\n--- " << e->name << "'s Turn ---\n";
        Utils::delay_ms(GameConfig::UI::DELAY_TURN_CHANGE);

        int damage = enemy_calculate_damage(e);
        std::cout << e->name << " attacks you for " << damage << " damage!\n";

        bool died = player_take_damage(p, damage);
        
        // Info kalkulasi pertahanan (opsional, untuk feedback user)
        int def_reduction = static_cast<int>(player_get_defense(p) * GameConfig::Combat::DEF_REDUCTION_FACTOR);
        std::cout << "You take " << damage << " damage after defense reduction.\n";

        if (died) {
            std::cout << "\nYou have been defeated...\n";
        }

        Utils::delay_ms(GameConfig::UI::DELAY_AFTER_ATTACK);
    }

    static bool use_item_menu(Player* p) {
        inventory_print_consumables(&p->inventory);
        std::cout << "\nEnter slot number (0 to cancel): ";

        int slot = Utils::get_int_input(0, GameConfig::Inventory::MAX_SLOTS);
        if (slot <= 0) return false;

        /* Convert display number to actual index */
        int actual_idx = -1;
        int display_count = 0;
        for (int i = 0; i < GameConfig::Inventory::MAX_SLOTS; i++) {
            InventorySlot* inv_slot = inventory_get_slot(&p->inventory, i);
            if (inv_slot && inv_slot->type == SLOT_ITEM && item_is_consumable(&inv_slot->data.item)) {
                display_count++;
                if (display_count == slot) {
                    actual_idx = i;
                    break;
                }
            }
        }

        if (actual_idx < 0) {
            std::cout << "Invalid slot.\n";
            Utils::delay_ms(GameConfig::UI::DELAY_MENU_TRANSITION);
            return false;
        }

        ResultCode result = player_use_item(p, actual_idx);
        if (result != RESULT_SUCCESS) {
            std::cout << "Cannot use that item.\n";
            return false;
        }

        Utils::delay_ms(GameConfig::UI::DELAY_AFTER_ATTACK);
        return true; 
    }

    static bool try_escape() {
        return Utils::random_chance(0.4f); // 40% base chance
    }

    /* ============================================
     * PUBLIC FUNCTIONS
     * ============================================ */

    BattleResult start(Player* player, Enemy* enemy) {
        if (!player || !enemy) return BATTLE_LOSE;

        /* Battle intro */
        Utils::clear_screen();
        Utils::print_header("BATTLE START!", 40);
        std::cout << "\n  A wild " << enemy->name << " appeared!\n";
        
        if (enemy->type == MONSTER_TYPE_ELITE) {
            std::cout << "  >> This is an ELITE monster! <<\n";
        } else if (enemy->type == MONSTER_TYPE_BOSS) {
            std::cout << "  >>> BOSS BATTLE! <<<\n";
        }
        
        Utils::delay_ms(GameConfig::UI::DELAY_BATTLE_START);

        /* Main battle loop */
        while (player_is_alive(player) && enemy_is_alive(enemy)) {
            print_battle_header(player, enemy);
            display_battle_menu();

            int choice = Utils::get_int_input(1, 3);

            if (choice == 1) { // Attack
                execute_player_attack(player, enemy);
            } 
            else if (choice == 2) { // Use Item
                if (!use_item_menu(player)) continue;
            } 
            else if (choice == 3) { // Run
                if (enemy->type == MONSTER_TYPE_BOSS) {
                    std::cout << "\nYou can't escape from a boss!\n";
                    Utils::delay_ms(GameConfig::UI::DELAY_MENU_TRANSITION);
                    continue;
                }

                if (try_escape()) {
                    std::cout << "\nYou escaped successfully!\n";
                    Utils::delay_ms(GameConfig::UI::DELAY_BATTLE_END);
                    return BATTLE_ESCAPE;
                } else {
                    std::cout << "\nCouldn't escape!\n";
                    Utils::delay_ms(GameConfig::UI::DELAY_AFTER_ATTACK);
                }
            } 
            else {
                continue;
            }

            /* Check if enemy died after player's turn */
            if (!enemy_is_alive(enemy)) break;

            /* Enemy turn */
            execute_enemy_attack(enemy, player);

            /* End of round effects */
            player_tick_buffs(player);
        }

        /* Battle result screen */
        Utils::clear_screen();

        if (player_is_alive(player)) {
            Utils::print_header("VICTORY!", 40);
            std::cout << "\nYou defeated " << enemy->name << "!\n";

            LootDrop loot = loot_generate(enemy);
            loot_print(&loot);
            loot_collect(player, &loot);

            Utils::delay_ms(GameConfig::UI::DELAY_BATTLE_END);
            return BATTLE_WIN;
        } else {
            Utils::print_header("DEFEAT", 40);
            std::cout << "\nYou were defeated by " << enemy->name << "...\n";
            Utils::delay_ms(GameConfig::UI::DELAY_BATTLE_END);
            return BATTLE_LOSE;
        }
    }

    BattleResult random_encounter(Player* player, MonsterType type) {
        Enemy enemy = enemy_create_random(type);
        return start(player, &enemy);
    }

} // namespace Battle