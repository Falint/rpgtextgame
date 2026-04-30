#include "monsters.hpp"
#include "../db/data_loader.hpp"
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
  for (auto& m : DataLoader::loadMonsters()) {
    monsters_[m.id] = std::move(m);
  }
  // Build byType index after insertion so pointers are stable
  for (auto &kv : monsters_) {
    byType_[kv.second.type].push_back(&monsters_[kv.first]);
  }
}
