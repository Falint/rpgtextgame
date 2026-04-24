#pragma once
#include "element.hpp"
#include <string>

enum class WeaponType { Fist = 0, Sword, Axe, Staff, Bow, Dagger };

inline const char *weaponTypeStr(WeaponType t) {
  switch (t) {
  case WeaponType::Sword:
    return "Sword";
  case WeaponType::Axe:
    return "Axe";
  case WeaponType::Staff:
    return "Staff";
  case WeaponType::Bow:
    return "Bow";
  case WeaponType::Dagger:
    return "Dagger";
  default:
    return "Fist";
  }
}

// Each tier above 1 adds 15% damage multiplicatively.
inline constexpr double TIER_BONUS_PER_LEVEL = 0.15;

struct Weapon {
  std::string id;
  std::string name;
  std::string description;
  WeaponType type = WeaponType::Fist;
  Element element = Element::None;
  int baseDamage = 0;
  int tier = 1;
  int maxTier = 5;

  // Total damage at current tier.
  int damage() const {
    double mult = 1.0 + (tier - 1) * TIER_BONUS_PER_LEVEL;
    return static_cast<int>(baseDamage * mult);
  }

  bool canUpgrade() const { return tier < maxTier; }

  // "Iron Sword +2" for tier 3, plain name for tier 1.
  std::string displayName() const {
    if (tier > 1)
      return name + " +" + std::to_string(tier - 1);
    return name;
  }
};
