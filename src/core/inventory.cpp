/**
 * @file inventory.cpp
 * @brief Inventory system implementation (C++ Version)
 */

#include "inventory.hpp"
#include "../config.hpp"
#include "../core/weapon.hpp"
#include "../core/item.hpp"
#include <iostream>
#include <variant>
#include <string>

namespace InventorySystem {

    void init(Inventory* inv) {
        if (!inv) return;

        for (int i = 0; i < GameConfig::Inventory::MAX_SLOTS; ++i) {
            inv->slots[i].type = SlotType::EMPTY;
            inv->slots[i].data = std::monostate{}; // Reset variant
            inv->slots[i].quantity = 0;
        }
        inv->used_slots = 0;
    }

    ResultCode add_weapon(Inventory* inv, const Weapon& weapon) {
        if (!inv) return ResultCode::FAIL;

        for (int i = 0; i < GameConfig::Inventory::MAX_SLOTS; ++i) {
            if (inv->slots[i].type == SlotType::EMPTY) {
                inv->slots[i].type = SlotType::WEAPON;
                inv->slots[i].data = weapon;
                inv->slots[i].quantity = 1;
                inv->used_slots++;
                return ResultCode::SUCCESS;
            }
        }
        return ResultCode::INVENTORY_FULL;
    }

    ResultCode add_item(Inventory* inv, const Item& item, int32_t quantity) {
        if (!inv || quantity <= 0) return ResultCode::FAIL;

        // 1. Coba tumpuk (stack) ke item yang sudah ada
        for (int i = 0; i < GameConfig::Inventory::MAX_SLOTS; ++i) {
            if (inv->slots[i].type == SlotType::ITEM) {
                // Mengambil referensi item dari variant untuk pengecekan
                const Item& existing = std::get<Item>(inv->slots[i].data);
                if (existing.id == item.id) {
                    int32_t new_qty = inv->slots[i].quantity + quantity;
                    inv->slots[i].quantity = (new_qty > GameConfig::Inventory::STACK_LIMIT) 
                                           ? GameConfig::Inventory::STACK_LIMIT 
                                           : new_qty;
                    return ResultCode::SUCCESS;
                }
            }
        }

        // 2. Jika tidak bisa ditumpuk, cari slot kosong
        for (int i = 0; i < GameConfig::Inventory::MAX_SLOTS; ++i) {
            if (inv->slots[i].type == SlotType::EMPTY) {
                inv->slots[i].type = SlotType::ITEM;
                inv->slots[i].data = item;
                inv->slots[i].quantity = quantity;
                inv->used_slots++;
                return ResultCode::SUCCESS;
            }
        }

        return ResultCode::INVENTORY_FULL;
    }

    void remove_at(Inventory* inv, int32_t index) {
        if (!inv || index < 0 || index >= GameConfig::Inventory::MAX_SLOTS) return;

        if (inv->slots[index].type != SlotType::EMPTY) {
            inv->slots[index].type = SlotType::EMPTY;
            inv->slots[index].data = std::monostate{}; // Membersihkan memori variant
            inv->slots[index].quantity = 0;
            inv->used_slots--;
        }
    }

    bool use_item_at(Inventory* inv, int32_t index) {
        if (!inv || index < 0 || index >= GameConfig::Inventory::MAX_SLOTS) return false;
        
        auto& slot = inv->slots[index];
        if (slot.type != SlotType::ITEM || slot.quantity <= 0) return false;

        slot.quantity--;
        if (slot.quantity == 0) {
            remove_at(inv, index);
        }
        return true;
    }

    int32_t count_item(const Inventory* inv, const std::string& item_id) {
        if (!inv) return 0;

        int32_t total = 0;
        for (int i = 0; i < GameConfig::Inventory::MAX_SLOTS; ++i) {
            if (inv->slots[i].type == SlotType::ITEM) {
                if (std::get<Item>(inv->slots[i].data).id == item_id) {
                    total += inv->slots[i].quantity;
                }
            }
        }
        return total;
    }

    ResultCode remove_item_by_id(Inventory* inv, const std::string& item_id, int32_t quantity) {
        if (!inv || quantity <= 0) return ResultCode::FAIL;
        if (count_item(inv, item_id) < quantity) return ResultCode::NOT_ENOUGH_MATERIALS;

        int32_t remaining = quantity;
        for (int i = 0; i < GameConfig::Inventory::MAX_SLOTS && remaining > 0; ++i) {
            if (inv->slots[i].type == SlotType::ITEM) {
                auto& item = std::get<Item>(inv->slots[i].data);
                if (item.id == item_id) {
                    if (inv->slots[i].quantity <= remaining) {
                        remaining -= inv->slots[i].quantity;
                        remove_at(inv, i);
                    } else {
                        inv->slots[i].quantity -= remaining;
                        remaining = 0;
                    }
                }
            }
        }
        return ResultCode::SUCCESS;
    }

    /* ============================================
     * DISPLAY FUNCTIONS
     * ============================================ */

    void print_all(const Inventory* inv) {
        if (!inv) return;

        std::cout << "\n--- Inventory (" << (int)inv->used_slots << "/" 
                  << GameConfig::Inventory::MAX_SLOTS << " slots) ---\n";

        int display_num = 1;
        for (int i = 0; i < GameConfig::Inventory::MAX_SLOTS; ++i) {
            const auto& slot = inv->slots[i];
            if (slot.type == SlotType::EMPTY) continue;

            std::cout << display_num++ << ". ";
            if (slot.type == SlotType::WEAPON) {
                weapon_print(&std::get<Weapon>(slot.data));
            } else {
                const auto& item = std::get<Item>(slot.data);
                std::cout << item.name << " (x" << slot.quantity << ")";
            }
            std::cout << "\n";
        }

        if (inv->used_slots == 0) std::cout << "  (Empty)\n";
        std::cout << "------------------------------\n";
    }

    void print_materials(const Inventory* inv) {
        if (!inv) return;
        std::cout << "\n--- Crafting Materials ---\n";
        bool found = false;
        for (int i = 0; i < GameConfig::Inventory::MAX_SLOTS; ++i) {
            if (inv->slots[i].type == SlotType::ITEM) {
                const auto& item = std::get<Item>(inv->slots[i].data);
                if (item.category == ItemCategory::MATERIAL) {
                    std::cout << "- " << item.name << " x" << inv->slots[i].quantity << "\n";
                    found = true;
                }
            }
        }
        if (!found) std::cout << "  (None)\n";
    }

} // namespace InventorySystem