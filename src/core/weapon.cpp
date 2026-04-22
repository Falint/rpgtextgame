/**
 * @file weapon.cpp
 * @brief Weapon system implementation (C++ Version)
 */

#include "weapon.hpp"
#include "element.hpp"
#include <iostream>
#include <string_view>

/* ============================================
 * WEAPON STRUCT METHODS
 * ============================================ */

void Weapon::apply_upgrade(int32_t damage_increase) {
    if (!can_upgrade()) {
        std::cout << "Weapon has reached its maximum tier!\n";
        return;
    }

    tier++;
    bonus_damage += damage_increase;

    /* Update harga jual: naik 20% setiap upgrade */
    sell_price = static_cast<int32_t>(sell_price * 1.2f);
    
    std::cout << name << " upgraded to Tier " << tier << "!\n";
}

void Weapon::print() const {
    std::cout << "[" << name;
    if (tier > 1) {
        std::cout << " +" << (tier - 1);
    }
    std::cout << "] DMG:" << get_total_damage();

    if (element != Element::NONE) {
        std::cout << " (" << ElementSystem::to_string(element) << ")";
    }
}

void Weapon::print_detailed() const {
    std::cout << "=== " << name;
    if (tier > 1) {
        std::cout << " +" << (tier - 1);
    }
    std::cout << " ===\n";

    std::cout << "Type: " << WeaponSystem::type_to_string(type) << "\n"
              << "Damage: " << get_total_damage();
                
    if (bonus_damage > 0) {
        std::cout << " (" << base_damage << " + " << bonus_damage << ")";
    }
    std::cout << "\n";

    std::cout << "Element: " << ElementSystem::to_string(element) << "\n"
              << "Tier: " << tier << "/" << max_tier << "\n"
              << "Sell Price: " << sell_price << " G\n"
              << "Description: " << description << "\n";
}

/* ============================================
 * WEAPON SYSTEM NAMESPACE
 * ============================================ */

namespace WeaponSystem {

    Weapon create(const std::string& template_id) {
        const WeaponTemplate* tmpl = Registry::get_weapon(template_id);
        
        if (!tmpl) {
            /* Default: Fists jika template tidak ditemukan */
            Weapon fists;
            fists.id = "fists";
            fists.name = "Fists";
            fists.description = "Your bare hands.";
            fists.type = WeaponType::FIST;
            fists.base_damage = 5;
            fists.max_tier = 1;
            return fists;
        }

        return create_from_template(tmpl);
    }

    Weapon create_from_template(const WeaponTemplate* tmpl) {
        Weapon w;
        if (!tmpl) return create("fists");

        w.id = tmpl->id;
        w.name = tmpl->name;
        w.description = tmpl->description;
        w.type = tmpl->type;
        w.element = tmpl->element;
        w.base_damage = tmpl->base_damage;
        w.bonus_damage = 0;
        w.tier = 1;
        w.max_tier = tmpl->max_tier;
        w.sell_price = tmpl->sell_price;

        return w;
    }

    int32_t get_damage(const Weapon* w) {
        return w ? w->get_total_damage() : 0;
    }

    std::string_view type_to_string(WeaponType type) {
        switch (type) {
            case WeaponType::FIST:   return "Fist";
            case WeaponType::SWORD:  return "Sword";
            case WeaponType::AXE:    return "Axe";
            case WeaponType::STAFF:  return "Staff";
            case WeaponType::BOW:    return "Bow";
            case WeaponType::DAGGER: return "Dagger";
            default:                 return "Unknown";
        }
    }

} // namespace WeaponSystem