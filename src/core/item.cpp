/**
 * @file item.cpp
 * @brief Item system implementation (C++ Version)
 */

#include "item.hpp"
#include "../utils/utils.hpp"
#include <iostream>
#include <string_view>

/* ============================================
 * ITEM STRUCT METHODS
 * ============================================ */

void Item::print() const {
    std::cout << "[" << name << "]";

    if (category == ItemCategory::CONSUMABLE) {
        switch (consumable_type) {
            case ConsumableType::HEAL:
                std::cout << " +" << value << " HP";
                break;
            case ConsumableType::BUFF_ATK:
                std::cout << " +" << value << "% ATK (" << duration << " turns)";
                break;
            case ConsumableType::BUFF_DEF:
                std::cout << " +" << value << "% DEF (" << duration << " turns)";
                break;
            case ConsumableType::FULL_RESTORE:
                std::cout << " Full HP";
                break;
            default:
                break;
        }
    }
}

void Item::print_detailed() const {
    std::cout << "=== " << name << " ===\n"
              << "Type: " << ItemSystem::category_to_string(category) << "\n";

    if (category == ItemCategory::CONSUMABLE) {
        std::cout << "Effect: " << ItemSystem::consumable_to_string(consumable_type) << "\n";
        if (value > 0) {
            std::cout << "Value: " << value << "\n";
        }
        if (duration > 0) {
            std::cout << "Duration: " << duration << " turns\n";
        }
    }

    std::cout << "Sell Price: " << sell_price << " G\n"
              << "Description: " << description << "\n";
}

/* ============================================
 * ITEM SYSTEM NAMESPACE
 * ============================================ */

namespace ItemSystem {

    Item create(const std::string& template_id) {
        const ItemTemplate* tmpl = Registry::get_item(template_id);
        
        if (!tmpl) {
            Item unknown;
            unknown.id = "unknown";
            unknown.name = "Unknown Item";
            unknown.description = "???";
            return unknown;
        }

        return create_from_template(tmpl);
    }

    Item create_from_template(const ItemTemplate* tmpl) {
        Item item;
        if (!tmpl) return create("unknown");

        item.id = tmpl->id;
        item.name = tmpl->name;
        item.description = tmpl->description;
        item.category = tmpl->category;
        item.consumable_type = tmpl->consumable_type;
        item.value = tmpl->value;
        item.duration = tmpl->duration;
        item.sell_price = tmpl->sell_price;

        return item;
    }

    std::string_view consumable_to_string(ConsumableType type) {
        switch (type) {
            case ConsumableType::HEAL:         return "Healing";
            case ConsumableType::BUFF_ATK:     return "ATK Buff";
            case ConsumableType::BUFF_DEF:     return "DEF Buff";
            case ConsumableType::CURE_POISON:  return "Cure";
            case ConsumableType::FULL_RESTORE: return "Full Restore";
            default:                           return "None";
        }
    }

    std::string_view category_to_string(ItemCategory cat) {
        switch (cat) {
            case ItemCategory::CONSUMABLE: return "Consumable";
            case ItemCategory::MATERIAL:   return "Material";
            case ItemCategory::KEY:        return "Key Item";
            default:                       return "Unknown";
        }
    }

} // namespace ItemSystem