#include "json_helpers.hpp"
#include <stdexcept>
#include <unordered_map>

namespace JsonHelpers {

WeaponType parseWeaponType(const std::string& s) {
    static const std::unordered_map<std::string, WeaponType> map = {
        {"Fist",   WeaponType::Fist},
        {"Sword",  WeaponType::Sword},
        {"Axe",    WeaponType::Axe},
        {"Staff",  WeaponType::Staff},
        {"Bow",    WeaponType::Bow},
        {"Dagger", WeaponType::Dagger},
    };
    auto it = map.find(s);
    if (it == map.end()) throw std::runtime_error("Unknown WeaponType: " + s);
    return it->second;
}

Element parseElement(const std::string& s) {
    static const std::unordered_map<std::string, Element> map = {
        {"None",  Element::None},
        {"Fire",  Element::Fire},
        {"Water", Element::Water},
        {"Earth", Element::Earth},
        {"Wind",  Element::Wind},
    };
    auto it = map.find(s);
    if (it == map.end()) throw std::runtime_error("Unknown Element: " + s);
    return it->second;
}

MonsterType parseMonsterType(const std::string& s) {
    static const std::unordered_map<std::string, MonsterType> map = {
        {"Normal", MonsterType::Normal},
        {"Elite",  MonsterType::Elite},
        {"Boss",   MonsterType::Boss},
    };
    auto it = map.find(s);
    if (it == map.end()) throw std::runtime_error("Unknown MonsterType: " + s);
    return it->second;
}

ItemCategory parseItemCategory(const std::string& s) {
    static const std::unordered_map<std::string, ItemCategory> map = {
        {"Consumable", ItemCategory::Consumable},
        {"Material",   ItemCategory::Material},
    };
    auto it = map.find(s);
    if (it == map.end()) throw std::runtime_error("Unknown ItemCategory: " + s);
    return it->second;
}

ConsumableType parseConsumableType(const std::string& s) {
    static const std::unordered_map<std::string, ConsumableType> map = {
        {"None",        ConsumableType::None},
        {"Heal",        ConsumableType::Heal},
        {"BuffAtk",     ConsumableType::BuffAtk},
        {"BuffDef",     ConsumableType::BuffDef},
        {"FullRestore", ConsumableType::FullRestore},
    };
    auto it = map.find(s);
    if (it == map.end()) throw std::runtime_error("Unknown ConsumableType: " + s);
    return it->second;
}

} // namespace JsonHelpers
