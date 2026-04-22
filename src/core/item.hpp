/**
 * @file item.hpp
 * @brief Item system with consumables and materials (C++ Version)
 */

#ifndef ITEM_HPP
#define ITEM_HPP

#include <string>
#include <string_view>

#include "../types.hpp"
#include "../data/registry.hpp"

/* Forward declaration */
struct Player;

/**
 * @brief Runtime item instance
 * * Menggunakan std::string untuk ID, Nama, dan Deskripsi guna mendukung
 * alokasi memori dinamis yang aman di dalam Inventory (std::variant).
 */
struct Item {
    std::string id;
    std::string name;
    std::string description;
    ItemCategory category;
    ConsumableType consumable_type;
    int32_t value;      /* Efek (misal: jumlah HP yang dipulihkan) */
    int32_t duration;   /* Durasi jika ini adalah buff */
    int32_t sell_price;

    /* ============================================
     * CONSTRUCTORS
     * ============================================ */
    
    Item() : category(ItemCategory::MATERIAL), 
             consumable_type(ConsumableType::NONE),
             value(0), duration(0), sell_price(0) {}

    /* ============================================
     * MEMBER FUNCTIONS
     * ============================================ */

    /**
     * @brief Cek tipe item
     */
    bool is_consumable() const { return category == ItemCategory::CONSUMABLE; }
    bool is_material() const   { return category == ItemCategory::MATERIAL; }

    /**
     * @brief Menampilkan info item ke konsol
     */
    void print() const;
    void print_detailed() const;

    /**
     * @brief Operator pembanding untuk stacking di inventory
     */
    bool operator==(const Item& other) const {
        return id == other.id;
    }
};

/**
 * Namespace ItemSystem untuk logika pembuatan dan utilitas item.
 */
namespace ItemSystem {

    /**
     * @brief Membuat instance Item berdasarkan ID template
     */
    Item create(const std::string& template_id);

    /**
     * @brief Membuat instance Item dari pointer template registry
     */
    Item create_from_template(const ItemTemplate* tmpl);

    /**
     * @brief Konversi enum ke representasi string
     */
    std::string_view consumable_to_string(ConsumableType type);
    std::string_view category_to_string(ItemCategory cat);

} // namespace ItemSystem

#endif // ITEM_HPP