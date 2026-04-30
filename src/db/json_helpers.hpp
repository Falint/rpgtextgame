#pragma once
#include <string>
#include "../domain/weapon.hpp"
#include "../domain/element.hpp"
#include "../domain/enemy.hpp"
#include "../domain/inventory.hpp"

namespace JsonHelpers {

// String → enum converters (throw std::runtime_error on unknown value)
WeaponType     parseWeaponType(const std::string& s);
Element        parseElement(const std::string& s);
MonsterType    parseMonsterType(const std::string& s);
ItemCategory   parseItemCategory(const std::string& s);
ConsumableType parseConsumableType(const std::string& s);

} // namespace JsonHelpers
