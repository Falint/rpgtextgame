#pragma once
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <stdexcept>
#include "../data/weapons.hpp"
#include "../data/items.hpp"
#include "../data/monsters.hpp"
#include "../data/upgrades.hpp"

/// Custom exception for data loading errors.
class DataLoadError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

namespace DataLoader {

/// Set the base directory for JSON data files.
/// Must be called before any Registry constructor.
/// Example: DataLoader::setBasePath("data/json");
void setBasePath(const std::string& path);

/// Returns the current base path.
const std::string& basePath();

// ── Weapon loading ───────────────────────────────────────────────────────
std::vector<WeaponTemplate> loadWeapons();
std::vector<std::pair<std::string, int>> loadWeaponShop();

// ── Item loading ─────────────────────────────────────────────────────────
std::vector<ItemTemplate> loadItems();
std::vector<std::pair<std::string, int>> loadItemShop();

// ── Monster loading ──────────────────────────────────────────────────────
std::vector<MonsterTemplate> loadMonsters();

// ── Upgrade loading ──────────────────────────────────────────────────────
std::map<std::string, std::vector<UpgradeRecipe>> loadUpgrades();

} // namespace DataLoader
