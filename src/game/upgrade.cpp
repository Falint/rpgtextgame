/**
 * @file upgrade.cpp
 * @brief Weapon upgrade system implementation (C++ Version)
 */

#include "upgrade.hpp"
#include "../core/inventory.hpp"
#include "../core/weapon.hpp"
#include "../data/registry.hpp"
#include "../utils/utils.hpp"
#include "../config.hpp"
#include <iostream>
#include <string>

namespace Upgrade {

    /* ============================================
     * INTERNAL HELPERS
     * ============================================ */

    static const UpgradeRecipe* get_recipe_for_weapon(const Weapon* w) {
        if (!w) return nullptr;
        return Registry::get_upgrade(w->id, w->tier);
    }

    static bool has_required_materials(const Inventory* inv, const UpgradeRecipe* recipe) {
        if (!inv || !recipe) return false;

        for (int i = 0; i < recipe->material_count; ++i) {
            int current_stock = inventory_count_item(inv, recipe->materials[i].item_id);
            if (current_stock < recipe->materials[i].quantity) {
                return false;
            }
        }
        return true;
    }

    static void consume_materials(Inventory* inv, const UpgradeRecipe* recipe) {
        if (!inv || !recipe) return;

        for (int i = 0; i < recipe->material_count; ++i) {
            inventory_remove_item_by_id(inv, recipe->materials[i].item_id,
                                      recipe->materials[i].quantity);
        }
    }

    /* ============================================
     * DISPLAY HELPERS
     * ============================================ */

    void print_requirements(const Weapon* weapon, const Player* player) {
        if (!weapon) return;

        const UpgradeRecipe* recipe = get_recipe_for_weapon(weapon);

        if (!recipe) {
            if (weapon->tier >= weapon->max_tier) {
                std::cout << "   This weapon is at maximum tier.\n";
            } else {
                std::cout << "   No upgrade path available.\n";
            }
            return;
        }

        std::cout << "   Upgrade: " << weapon->name << " +" << (weapon->tier - 1) 
                  << " -> +" << (recipe->to_tier - 1) << "\n";
        
        int current_dmg = weapon_get_damage(weapon);
        std::cout << "   Damage: " << current_dmg << " -> " 
                  << (current_dmg + recipe->damage_bonus) 
                  << " (+" << recipe->damage_bonus << ")\n";

        std::cout << "\n   Requirements:\n";
        std::cout << "    Gold: " << recipe->gold_cost;
        
        if (player && player->gold >= recipe->gold_cost) {
            std::cout << " [OK]";
        } else {
            int missing = recipe->gold_cost - (player ? player->gold : 0);
            std::cout << " [NEED " << missing << " more]";
        }
        std::cout << "\n";

        for (int i = 0; i < recipe->material_count; ++i) {
            const ItemTemplate* item = Registry::get_item(recipe->materials[i].item_id);
            std::string item_name = item ? item->name : recipe->materials[i].item_id;
            
            int have = player ? inventory_count_item(&player->inventory, recipe->materials[i].item_id) : 0;
            int need = recipe->materials[i].quantity;

            std::cout << "    " << item_name << ": " << have << "/" << need;
            if (have >= need) {
                std::cout << " [OK]\n";
            } else {
                std::cout << " [NEED " << (need - have) << " more]\n";
            }
        }
    }

    static void print_upgradeable_list(const Player* p) {
        std::cout << "=== Upgradeable Weapons ===\n\n";
        bool found = false;

        // Check equipped weapon
        if (p->has_weapon && weapon_can_upgrade(&p->equipped_weapon)) {
            std::cout << "  [E] ";
            weapon_print(&p->equipped_weapon);
            std::cout << " (EQUIPPED)\n";

            if (const UpgradeRecipe* recipe = get_recipe_for_weapon(&p->equipped_weapon)) {
                bool ready = p->gold >= recipe->gold_cost && has_required_materials(&p->inventory, recipe);
                std::cout << "      -> Tier " << recipe->to_tier << " (" 
                          << (ready ? "READY" : "missing materials") << ")\n";
            }
            found = true;
        }

        // Check inventory weapons
        int display_num = 1;
        for (int i = 0; i < GameConfig::Inventory::MAX_SLOTS; ++i) {
            InventorySlot* slot = inventory_get_slot(const_cast<Inventory*>(&p->inventory), i);
            if (!slot || slot->type != SLOT_WEAPON) continue;

            Weapon* w = &slot->data.weapon;
            if (!weapon_can_upgrade(w)) continue;

            std::cout << "  " << display_num++ << ". ";
            weapon_print(w);
            std::cout << " [Slot " << i << "]\n";

            if (const UpgradeRecipe* recipe = get_recipe_for_weapon(w)) {
                bool ready = p->gold >= recipe->gold_cost && has_required_materials(&p->inventory, recipe);
                std::cout << "      -> Tier " << recipe->to_tier << " (" 
                          << (ready ? "READY" : "missing materials") << ")\n";
            }
            found = true;
        }

        if (!found) {
            std::cout << "  No weapons can be upgraded.\n"
                      << "  (Collect materials from monsters or buy upgradeable weapons)\n";
        }
        std::cout << std::endl;
    }

    /* ============================================
     * PUBLIC FUNCTIONS
     * ============================================ */

    bool can_upgrade_weapon(const Player* player, int weapon_slot) {
        if (!player) return false;

        const Weapon* w;
        if (weapon_slot < 0) {
            if (!player->has_weapon) return false;
            w = &player->equipped_weapon;
        } else {
            InventorySlot* slot = inventory_get_slot(const_cast<Inventory*>(&player->inventory), weapon_slot);
            if (!slot || slot->type != SLOT_WEAPON) return false;
            w = &slot->data.weapon;
        }

        if (!weapon_can_upgrade(w)) return false;

        const UpgradeRecipe* recipe = get_recipe_for_weapon(w);
        if (!recipe) return false;

        return (player->gold >= recipe->gold_cost && has_required_materials(&player->inventory, recipe));
    }

    ResultCode perform_upgrade(Player* player, int weapon_slot) {
        if (!player) return RESULT_FAIL;

        Weapon* w;
        if (weapon_slot < 0) {
            if (!player->has_weapon) return RESULT_INVALID_SLOT;
            w = &player->equipped_weapon;
        } else {
            InventorySlot* slot = inventory_get_slot(&player->inventory, weapon_slot);
            if (!slot || slot->type != SLOT_WEAPON) return RESULT_INVALID_SLOT;
            w = &slot->data.weapon;
        }

        if (!weapon_can_upgrade(w)) return RESULT_MAX_TIER_REACHED;

        const UpgradeRecipe* recipe = get_recipe_for_weapon(w);
        if (!recipe) return RESULT_CANNOT_UPGRADE;

        if (player->gold < recipe->gold_cost) return RESULT_NOT_ENOUGH_GOLD;
        if (!has_required_materials(&player->inventory, recipe)) return RESULT_NOT_ENOUGH_MATERIALS;

        // Apply Upgrade
        player_spend_gold(player, recipe->gold_cost);
        consume_materials(&player->inventory, recipe);
        weapon_apply_upgrade(w, recipe->damage_bonus);

        return RESULT_SUCCESS;
    }

    MenuResult enter_upgrade_menu(Player* player) {
        if (!player) return MenuResult::BACK;

        while (true) {
            Utils::clear_screen();
            Utils::print_header("WEAPON UPGRADE", 50);
            std::cout << "\n  Your Gold: " << player->gold << "\n\n";

            print_upgradeable_list(player);
            inventory_print_materials(&player->inventory);

            std::cout << "\n--- Upgrade Menu ---\n"
                      << "1. Upgrade Equipped Weapon\n"
                      << "2. Upgrade Inventory Weapon\n"
                      << "3. View Details\n"
                      << "4. Back\n"
                      << "Choice: ";

            int choice = Utils::get_int_input(1, 4);
            if (choice == 4) break;

            if (choice == 1) {
                if (!player->has_weapon) {
                    std::cout << "No weapon equipped.\n";
                    Utils::delay_ms(GameConfig::UI::DELAY_MENU_TRANSITION);
                    continue;
                }
                std::cout << "\n";
                print_requirements(&player->equipped_weapon, player);
                std::cout << "\nUpgrade?";
                if (Utils::get_yes_no()) {
                    ResultCode res = perform_upgrade(player, -1);
                    if (res == RESULT_SUCCESS) std::cout << "\nUpgrade successful!\n";
                    else std::cout << "\nUpgrade failed (Error Code: " << res << ")\n";
                }
                Utils::delay_ms(GameConfig::UI::DELAY_MENU_TRANSITION);
            } 
            else if (choice == 2) {
                std::cout << "\nEnter weapon slot number (0 to cancel): ";
                int slot_num = Utils::get_int_input(0, GameConfig::Inventory::MAX_SLOTS);
                if (slot_num == 0) continue;

                int actual_slot = -1, display_count = 0;
                for (int i = 0; i < GameConfig::Inventory::MAX_SLOTS; ++i) {
                    InventorySlot* s = inventory_get_slot(&player->inventory, i);
                    if (s && s->type == SLOT_WEAPON && weapon_can_upgrade(&s->data.weapon)) {
                        if (++display_count == slot_num) {
                            actual_slot = i;
                            break;
                        }
                    }
                }

                if (actual_slot >= 0) {
                    InventorySlot* s = inventory_get_slot(&player->inventory, actual_slot);
                    print_requirements(&s->data.weapon, player);
                    std::cout << "\nUpgrade?";
                    if (Utils::get_yes_no()) {
                        if (perform_upgrade(player, actual_slot) == RESULT_SUCCESS)
                            std::cout << "\nUpgrade successful!\n";
                    }
                } else {
                    std::cout << "Invalid slot.\n";
                }
                Utils::delay_ms(GameConfig::UI::DELAY_MENU_TRANSITION);
            }
            else if (choice == 3) {
                std::cout << "\nEnter 'E' for equipped or slot number: ";
                std::string input;
                std::cin >> input;
                if (input == "E" || input == "e") {
                    if (player->has_weapon) {
                        weapon_print_detailed(&player->equipped_weapon);
                        print_requirements(&player->equipped_weapon, player);
                    }
                }
                Utils::wait_for_enter();
            }
        }
        return MenuResult::BACK;
    }

} // namespace Upgrade