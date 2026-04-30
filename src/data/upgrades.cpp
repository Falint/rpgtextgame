#include "upgrades.hpp"
#include "../db/data_loader.hpp"

UpgradeRegistry::UpgradeRegistry() { load(); }

const UpgradeRecipe* UpgradeRegistry::getRecipe(const std::string& wid, int fromTier) const {
    auto it = recipes_.find(wid);
    if (it == recipes_.end()) return nullptr;
    for (auto& r : it->second) if (r.fromTier == fromTier) return &r;
    return nullptr;
}

std::vector<UpgradeRecipe> UpgradeRegistry::getAllRecipesForWeapon(const std::string& wid) const {
    auto it = recipes_.find(wid);
    return it == recipes_.end() ? std::vector<UpgradeRecipe>{} : it->second;
}

std::vector<std::string> UpgradeRegistry::getUpgradeableWeaponIDs() const {
    std::vector<std::string> v;
    for (auto& kv : recipes_) v.push_back(kv.first);
    return v;
}

void UpgradeRegistry::load() {
    recipes_ = DataLoader::loadUpgrades();
}