/**
 * @file loot.cpp
 * @brief Loot drop system implementation (C++ Version)
 */

#include "loot.hpp"
#include "inventory.hpp"
#include "../utils/utils.hpp" // Asumsi mengandung fungsi random_range dan random_chance
#include <iostream>

namespace LootSystem {

    LootDrop generate(const Enemy* enemy) {
        LootDrop loot; // Otomatis memanggil constructor (gold=0, item_count=0)

        if (!enemy || !enemy->tmpl) {
            // Fallback jika template tidak ditemukan
            loot.gold = Utils::random_range(
                GameConfig::Loot::GOLD_DROP_MIN, 
                GameConfig::Loot::GOLD_DROP_MAX
            );
            return loot;
        }

        // 1. Roll Gold menggunakan fungsi member dari Enemy
        loot.gold = enemy->roll_gold_drop();

        // 2. Roll for item drops berdasarkan template musuh
        const MonsterTemplate* tmpl = enemy->tmpl;

        for (int i = 0; i < tmpl->loot_count && loot.item_count < GameConfig::Loot::MAX_ENTRIES; ++i) {
            if (Utils::random_chance(tmpl->loot[i].drop_chance)) {
                Item dropped = ItemSystem::create(tmpl->loot[i].item_id);
                
                // Cek validitas item menggunakan operator ! pada string id
                if (!dropped.id.empty()) {
                    loot.items[loot.item_count++] = std::move(dropped); // Menggunakan move untuk efisiensi
                }
            }
        }

        return loot;
    }

    void collect(Player* player, const LootDrop* loot) {
        if (!player || !loot) return;

        // Tambah Gold
        player->gold += loot->gold;
        std::cout << "Gained " << loot->gold << " gold.\n";

        // Tambah setiap item ke inventory
        for (int i = 0; i < loot->item_count; ++i) {
            ResultCode result = InventorySystem::add_item(&player->inventory, loot->items[i], 1);
            
            if (result == ResultCode::INVENTORY_FULL) {
                std::cout << "[!] Inventory full! Could not pick up: " << loot->items[i].name << "\n";
            } else {
                std::cout << "Found item: " << loot->items[i].name << "\n";
            }
        }
    }

    void print_summary(const LootDrop* loot) {
        if (!loot) return;

        std::cout << "\n==============================\n"
                  << "        VICTORY LOOT          \n"
                  << "==============================\n"
                  << "  Gold: " << loot->gold << " G\n";

        if (loot->item_count > 0) {
            std::cout << "  Items dropped:\n";
            for (int i = 0; i < loot->item_count; ++i) {
                std::cout << "    > " << loot->items[i].name << "\n";
            }
        } else {
            std::cout << "  No items found.\n";
        }
        std::cout << "==============================\n";
    }

} // namespace LootSystem