/**
 * @file inventory.hpp
 * @brief Inventory system for managing items and weapons (C++ Version)
 */

#ifndef INVENTORY_HPP
#define INVENTORY_HPP

#include <array>
#include <string>
#include <variant>

#include "../types.hpp"
#include "../config.hpp"
#include "item.hpp"
#include "weapon.hpp"

/**
 * @brief Single inventory slot
 * Menggunakan std::variant untuk keamanan tipe data yang memiliki non-trivial destructor.
 */
struct InventorySlot {
    SlotType type;
    std::variant<std::monostate, Weapon, Item> data; 
    int32_t quantity; /* Untuk item stackable; senjata selalu 1 */

    // Constructor sederhana untuk memastikan slot baru selalu kosong
    InventorySlot() : type(SlotType::EMPTY), data(std::monostate{}), quantity(0) {}
};

/**
 * @brief Inventory container
 */
struct Inventory {
    std::array<InventorySlot, GameConfig::Inventory::MAX_SLOTS> slots;
    int32_t used_slots; 

    Inventory() : used_slots(0) {}
};

/**
 * Mengelompokkan fungsi inventory ke dalam namespace InventorySystem
 * agar pemanggilan lebih bersih: InventorySystem::add_item(...)
 */
namespace InventorySystem {

    /**
     * @brief Inisialisasi inventory kosong
     */
    void init(Inventory* inv);

    /**
     * @brief Tambah senjata ke inventory
     */
    ResultCode add_weapon(Inventory* inv, const Weapon& weapon);

    /**
     * @brief Tambah item ke inventory (otomatis menumpuk/stack jika ID sama)
     */
    ResultCode add_item(Inventory* inv, const Item& item, int32_t quantity);

    /**
     * @brief Hapus isi slot berdasarkan index
     */
    void remove_at(Inventory* inv, int32_t index);

    /**
     * @brief Gunakan satu item dari slot (mengurangi quantity)
     * @return true jika berhasil, false jika slot bukan item atau kosong
     */
    bool use_item_at(Inventory* inv, int32_t index);

    /**
     * @brief Mendapatkan pointer ke slot berdasarkan index
     */
    InventorySlot* get_slot(Inventory* inv, int32_t index);

    /**
     * @brief Menghitung total jumlah item berdasarkan ID
     */
    int32_t count_item(const Inventory* inv, const std::string& item_id);

    /**
     * @brief Hapus item berdasarkan ID dalam jumlah tertentu (untuk crafting/upgrade)
     */
    ResultCode remove_item_by_id(Inventory* inv, const std::string& item_id, int32_t quantity);

    /**
     * @brief Cari index slot pertama yang berisi item ID tertentu
     */
    int32_t find_item(const Inventory* inv, const std::string& item_id);

    /**
     * @brief Cek status kepenuhan inventory
     */
    bool is_full(const Inventory* inv);
    int32_t free_slots_count(const Inventory* inv);

    /* ============================================
     * DISPLAY FUNCTIONS
     * ============================================ */
    
    void print_all(const Inventory* inv);
    void print_weapons(const Inventory* inv);
    void print_consumables(const Inventory* inv);
    void print_materials(const Inventory* inv);

} // namespace InventorySystem

#endif // INVENTORY_HPP