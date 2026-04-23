/**
 * @file loot.hpp
 * @brief Loot drop system (C++ Version)
 */

#ifndef LOOT_HPP
#define LOOT_HPP

#include <array>
#include <vector>

#include "../types.hpp"
#include "../config.hpp"
#include "enemy.hpp"
#include "item.hpp"
#include "player.hpp"

/**
 * @brief Generated loot from defeated enemy
 * Menggunakan std::array dengan ukuran dari GameConfig.
 */
struct LootDrop {
    int32_t gold;
    std::array<Item, GameConfig::Loot::MAX_ENTRIES> items;
    int32_t item_count;

    // Constructor untuk memastikan loot dimulai dari keadaan bersih
    LootDrop() : gold(0), item_count(0) {}
};

/**
 * Namespace LootSystem mengelola logika drop rate dan distribusi item.
 */
namespace LootSystem {

    /**
     * @brief Menghasilkan loot berdasarkan tabel drop musuh yang dikalahkan.
     * Menggunakan RNG untuk menentukan apakah item drop atau tidak.
     * @param enemy Pointer ke musuh yang baru saja dikalahkan.
     * @return LootDrop Objek yang berisi emas dan daftar item.
     */
    LootDrop generate(const Enemy* enemy);

    /**
     * @brief Mentransfer emas dan item dari loot ke dalam data pemain.
     * Fungsi ini akan memanggil InventorySystem::add_item secara otomatis.
     * @param player Pointer ke pemain.
     * @param loot Pointer ke loot yang dihasilkan.
     */
    void collect(Player* player, const LootDrop* loot);

    /**
     * @brief Mencetak ringkasan hasil jarahan ke konsol dengan format UI yang rapi.
     */
    void print_summary(const LootDrop* loot);

} // namespace LootSystem

#endif // LOOT_HPP