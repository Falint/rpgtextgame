#include "data_loader.hpp"
#include "json_helpers.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

// ─── Internal state ──────────────────────────────────────────────────────────

namespace {
    std::string g_basePath = "data/json";

    /// Read and parse a JSON file. Throws DataLoadError on failure.
    json readJsonFile(const std::string& filename) {
        std::string path = g_basePath + "/" + filename;
        std::ifstream file(path);
        if (!file.is_open()) {
            throw DataLoadError("Cannot open data file: " + path);
        }
        try {
            json j;
            file >> j;
            return j;
        } catch (const json::parse_error& e) {
            throw DataLoadError("JSON parse error in " + path + ": " + e.what());
        }
    }
} // anonymous namespace

// ─── Public API ──────────────────────────────────────────────────────────────

namespace DataLoader {

void setBasePath(const std::string& path) {
    g_basePath = path;
}

const std::string& basePath() {
    return g_basePath;
}

// ── Weapons ──────────────────────────────────────────────────────────────────

std::vector<WeaponTemplate> loadWeapons() {
    auto root = readJsonFile("weapons.json");
    std::vector<WeaponTemplate> result;

    if (!root.contains("weapons") || !root["weapons"].is_array()) {
        throw DataLoadError("weapons.json: missing or invalid 'weapons' array");
    }

    for (const auto& w : root["weapons"]) {
        WeaponTemplate t;
        t.id          = w.at("id").get<std::string>();
        t.name        = w.at("name").get<std::string>();
        t.description = w.at("description").get<std::string>();
        t.type        = JsonHelpers::parseWeaponType(w.at("type").get<std::string>());
        t.element     = JsonHelpers::parseElement(w.at("element").get<std::string>());
        t.baseDamage  = w.at("baseDamage").get<int>();
        t.buyPrice    = w.at("buyPrice").get<int>();
        t.sellPrice   = w.at("sellPrice").get<int>();
        t.maxTier     = w.at("maxTier").get<int>();
        result.push_back(std::move(t));
    }

    return result;
}

std::vector<std::pair<std::string, int>> loadWeaponShop() {
    auto root = readJsonFile("weapons.json");
    std::vector<std::pair<std::string, int>> result;

    if (!root.contains("shop") || !root["shop"].is_array()) {
        throw DataLoadError("weapons.json: missing or invalid 'shop' array");
    }

    // Sort by sortOrder to maintain display order
    auto shopArr = root["shop"];
    std::vector<json> entries(shopArr.begin(), shopArr.end());
    std::sort(entries.begin(), entries.end(), [](const json& a, const json& b) {
        return a.at("sortOrder").get<int>() < b.at("sortOrder").get<int>();
    });

    for (const auto& s : entries) {
        result.emplace_back(
            s.at("weaponId").get<std::string>(),
            s.at("stock").get<int>()
        );
    }

    return result;
}

// ── Items ────────────────────────────────────────────────────────────────────

std::vector<ItemTemplate> loadItems() {
    auto root = readJsonFile("items.json");
    std::vector<ItemTemplate> result;

    if (!root.contains("items") || !root["items"].is_array()) {
        throw DataLoadError("items.json: missing or invalid 'items' array");
    }

    for (const auto& i : root["items"]) {
        ItemTemplate t;
        t.id             = i.at("id").get<std::string>();
        t.name           = i.at("name").get<std::string>();
        t.description    = i.at("description").get<std::string>();
        t.category       = JsonHelpers::parseItemCategory(i.at("category").get<std::string>());
        t.consumableType = JsonHelpers::parseConsumableType(i.at("consumableType").get<std::string>());
        t.value          = i.at("value").get<int>();
        t.duration       = i.at("duration").get<int>();
        t.buyPrice       = i.at("buyPrice").get<int>();
        t.sellPrice      = i.at("sellPrice").get<int>();
        t.stackable      = i.at("stackable").get<bool>();
        result.push_back(std::move(t));
    }

    return result;
}

std::vector<std::pair<std::string, int>> loadItemShop() {
    auto root = readJsonFile("items.json");
    std::vector<std::pair<std::string, int>> result;

    if (!root.contains("shop") || !root["shop"].is_array()) {
        throw DataLoadError("items.json: missing or invalid 'shop' array");
    }

    auto shopArr = root["shop"];
    std::vector<json> entries(shopArr.begin(), shopArr.end());
    std::sort(entries.begin(), entries.end(), [](const json& a, const json& b) {
        return a.at("sortOrder").get<int>() < b.at("sortOrder").get<int>();
    });

    for (const auto& s : entries) {
        result.emplace_back(
            s.at("itemId").get<std::string>(),
            s.at("stock").get<int>()
        );
    }

    return result;
}

// ── Monsters ─────────────────────────────────────────────────────────────────

std::vector<MonsterTemplate> loadMonsters() {
    auto root = readJsonFile("monsters.json");
    std::vector<MonsterTemplate> result;

    if (!root.contains("monsters") || !root["monsters"].is_array()) {
        throw DataLoadError("monsters.json: missing or invalid 'monsters' array");
    }

    for (const auto& m : root["monsters"]) {
        MonsterTemplate t;
        t.id      = m.at("id").get<std::string>();
        t.name    = m.at("name").get<std::string>();
        t.type    = JsonHelpers::parseMonsterType(m.at("type").get<std::string>());
        t.element = JsonHelpers::parseElement(m.at("element").get<std::string>());
        t.baseHP  = m.at("baseHP").get<int>();
        t.baseAtk = m.at("baseAtk").get<int>();
        t.baseDef = m.at("baseDef").get<int>();
        t.goldMin = m.at("goldMin").get<int>();
        t.goldMax = m.at("goldMax").get<int>();

        // Parse nested loot table
        if (m.contains("loot") && m["loot"].is_array()) {
            for (const auto& l : m["loot"]) {
                LootEntry entry;
                entry.itemID     = l.at("itemId").get<std::string>();
                entry.dropChance = l.at("dropChance").get<float>();
                t.lootTable.push_back(std::move(entry));
            }
        }

        result.push_back(std::move(t));
    }

    return result;
}

// ── Upgrades ─────────────────────────────────────────────────────────────────

std::map<std::string, std::vector<UpgradeRecipe>> loadUpgrades() {
    auto root = readJsonFile("upgrades.json");
    std::map<std::string, std::vector<UpgradeRecipe>> result;

    if (!root.contains("upgrades") || !root["upgrades"].is_array()) {
        throw DataLoadError("upgrades.json: missing or invalid 'upgrades' array");
    }

    for (const auto& u : root["upgrades"]) {
        UpgradeRecipe r;
        r.weaponID    = u.at("weaponId").get<std::string>();
        r.fromTier    = u.at("fromTier").get<int>();
        r.toTier      = u.at("toTier").get<int>();
        r.goldCost    = u.at("goldCost").get<int>();
        r.damageBonus = u.at("damageBonus").get<int>();

        // Parse nested materials
        if (u.contains("materials") && u["materials"].is_array()) {
            for (const auto& mat : u["materials"]) {
                UpgradeMaterial m;
                m.itemID   = mat.at("itemId").get<std::string>();
                m.quantity = mat.at("quantity").get<int>();
                r.materials.push_back(std::move(m));
            }
        }

        result[r.weaponID].push_back(std::move(r));
    }

    return result;
}

} // namespace DataLoader
