/**
 * @file weapon.hpp
 * @brief Weapon system with upgrade support (C++ Version)
 */

#ifndef WEAPON_HPP
#define WEAPON_HPP

#include <string>
#include <string_view>

#include "../types.hpp"
#include "../data/registry.hpp"

/**
 * @brief Runtime weapon instance
 */
struct Weapon {
    /* ============================================
     * DATA MEMBERS
     * ============================================ */
    std::string id;
    std::string name;
    std::string description;
    WeaponType type;
    Element element;
    int32_t base_damage;
    int32_t bonus_damage;
    int32_t tier;
    int32_t max_tier;
    int32_t sell_price;

    /* ============================================
     * CONSTRUCTOR
     * ============================================ */
    
    Weapon() : type(WeaponType::FIST), element(Element::NONE),
               base_damage(0), bonus_damage(0), tier(1), 
               max_tier(1), sell_price(0) {}

    /* ============================================
     * MEMBER FUNCTIONS
     * ============================================ */

    /**
     * @brief Mendapatkan total damage saat ini (Base + Bonus)
     */
    int32_t get_total_damage() const { return base_damage + bonus_damage; }

    /**
     * @brief Cek apakah senjata masih bisa di-upgrade
     */
    bool can_upgrade() const { return tier < max_tier; }

    /**
     * @brief Menaikkan tier senjata dan menambah damage
     */
    void apply_upgrade(int32_t damage_increase);

    /**
     * @brief Menampilkan info senjata
     */
    void print() const;
    void print_detailed() const;
};

/* ============================================
 * WEAPON SYSTEM NAMESPACE
 * ============================================ */

namespace WeaponSystem {

    /**
     * @brief Factory functions untuk membuat senjata
     */
    Weapon create(const std::string& template_id);
    Weapon create_from_template(const WeaponTemplate* tmpl);

    /**
     * @brief Helper konversi tipe ke string
     */
    std::string_view type_to_string(WeaponType type);

    /**
     * @brief Fungsi statis untuk kalkulasi damage (jika diperlukan secara global)
     */
    int32_t get_damage(const Weapon* w);

} // namespace WeaponSystem

#endif // WEAPON_HPP