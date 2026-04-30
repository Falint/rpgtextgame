#include "monsters.hpp"
#include <cstdlib>

MonsterRegistry::MonsterRegistry() { load(); }

const MonsterTemplate *MonsterRegistry::getByID(const std::string &id) const {
  auto it = monsters_.find(id);
  return it == monsters_.end() ? nullptr : &it->second;
}

std::vector<const MonsterTemplate *>
MonsterRegistry::getByType(MonsterType t) const {
  std::vector<const MonsterTemplate *> v;
  auto it = byType_.find(t);
  if (it != byType_.end())
    for (auto *p : it->second)
      v.push_back(p);
  return v;
}

const MonsterTemplate *MonsterRegistry::getRandom(MonsterType t) const {
  auto it = byType_.find(t);
  if (it == byType_.end() || it->second.empty())
    return nullptr;
  return it->second[std::rand() % it->second.size()];
}

void MonsterRegistry::load() {
  std::vector<MonsterTemplate> data = {
      // Normal
      {"slime",
       "Slime",
       MonsterType::Normal,
       Element::Water,
       30,
       8,
       2,
       10,
       20,
       {{"slime_gel", 0.5f}}},
      {"goblin",
       "Goblin",
       MonsterType::Normal,
       Element::None,
       40,
       12,
       4,
       15,
       30,
       {{"goblin_ear", 0.3f}}},
      {"wolf",
       "Wild Wolf",
       MonsterType::Normal,
       Element::Wind,
       35,
       15,
       3,
       12,
       25,
       {{"wolf_pelt", 0.4f}, {"ember_stone", 0.9f}}},
      {"skeleton",
       "Skeleton",
       MonsterType::Normal,
       Element::None,
       45,
       14,
       6,
       18,
       35,
       {{"bone_fragment", 0.6f}}},
      {"bat",
       "Giant Bat",
       MonsterType::Normal,
       Element::Wind,
       25,
       10,
       2,
       8,
       18,
       {{"bat_wing", 0.45f}}},
      // Elite
      {"goblin_chief",
       "Goblin Chief",
       MonsterType::Elite,
       Element::Fire,
       80,
       22,
       10,
       40,
       70,
       {{"goblin_ear", 1.0f}, {"iron_ore", 0.5f}}},
      {"dire_wolf",
       "Dire Wolf",
       MonsterType::Elite,
       Element::Wind,
       70,
       28,
       8,
       35,
       60,
       {{"wolf_pelt", 1.0f}, {"wolf_fang", 0.4f}}},
      {"flame_elemental",
       "Flame Elemental",
       MonsterType::Elite,
       Element::Fire,
       60,
       35,
       5,
       45,
       80,
       {{"fire_essence", 0.6f}}},
      // Boss
      {"dragon",
       "Ancient Dragon",
       MonsterType::Boss,
       Element::Fire,
       200,
       45,
       20,
       150,
       300,
       {{"dragon_scale", 1.0f}, {"fire_essence", 1.0f}}},
      {"lich",
       "Lich King",
       MonsterType::Boss,
       Element::None,
       180,
       40,
       15,
       120,
       250,
       {{"dark_crystal", 1.0f}, {"bone_fragment", 1.0f}}},
  };

  for (auto &m : data) {
    monsters_[m.id] = m;
  }
  // Build byType index after insertion so pointers are stable
  for (auto &kv : monsters_) {
    byType_[kv.second.type].push_back(&monsters_[kv.first]);
  }
}
