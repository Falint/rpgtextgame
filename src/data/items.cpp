#include "items.hpp"

ItemRegistry::ItemRegistry() { load(); initShopStock(); }

const ItemTemplate* ItemRegistry::getByID(const std::string& id) const {
    auto it = items_.find(id);
    return it == items_.end() ? nullptr : &it->second;
}

std::vector<ShopItemEntry> ItemRegistry::getShopItems() const {
    static const std::vector<std::string> order = {
        "small_potion","medium_potion","large_potion","full_restore",
        "atk_elixir","def_elixir","power_surge","iron_skin","antidote"
    };
    std::vector<ShopItemEntry> result;
    for (auto& id : order) {
        auto it = items_.find(id);
        if (it != items_.end() && it->second.buyPrice > 0) {
            int stk = 0;
            auto sit = shopStock_.find(id);
            if (sit != shopStock_.end()) stk = sit->second;
            result.push_back({&it->second, stk});
        }
    }
    return result;
}

int ItemRegistry::getStock(const std::string& id) const {
    auto it = shopStock_.find(id);
    return it == shopStock_.end() ? 0 : it->second;
}

bool ItemRegistry::decrementStock(const std::string& id, int amount) {
    auto it = shopStock_.find(id);
    if (it == shopStock_.end()) return false;
    if (it->second == -1) return true;
    if (it->second >= amount) { it->second -= amount; return true; }
    return false;
}

void ItemRegistry::load() {
    using IC = ItemCategory;
    using CT = ConsumableType;
    auto add = [&](ItemTemplate t){ items_[t.id] = t; };

    // Healing
    add({"small_potion",  "Small Potion",     "Restores 30 HP.",             IC::Consumable, CT::Heal,        30, 0,  25,  10, true});
    add({"medium_potion", "Medium Potion",    "Restores 60 HP.",             IC::Consumable, CT::Heal,        60, 0,  50,  20, true});
    add({"large_potion",  "Large Potion",     "Restores 100 HP.",            IC::Consumable, CT::Heal,       100, 0, 100,  40, true});
    add({"full_restore",  "Full Restore",     "Completely restores HP.",     IC::Consumable, CT::FullRestore, 999,0, 300, 120, true});
    // Buffs
    add({"atk_elixir",   "Attack Elixir",    "ATK +25% for 3 turns.",       IC::Consumable, CT::BuffAtk,     25, 3,  60,  25, true});
    add({"def_elixir",   "Defense Elixir",   "DEF +25% for 3 turns.",       IC::Consumable, CT::BuffDef,     25, 3,  60,  25, true});
    add({"power_surge",  "Power Surge",      "ATK +50% for 2 turns.",       IC::Consumable, CT::BuffAtk,     50, 2, 120,  50, true});
    add({"iron_skin",    "Iron Skin Potion", "DEF +50% for 2 turns.",       IC::Consumable, CT::BuffDef,     50, 2, 120,  50, true});
    add({"antidote",     "Antidote",         "Cures poison.",               IC::Consumable, CT::None,         0, 0,  30,  12, true});
    // Materials — common
    add({"slime_gel",     "Slime Gel",        "Gooey substance.",            IC::Material, CT::None, 0,0,0,15,true});
    add({"goblin_fang",   "Goblin Fang",      "Sharp fang.",                 IC::Material, CT::None, 0,0,0,20,true});
    add({"goblin_ear",    "Goblin Ear",       "Pointy ear.",                 IC::Material, CT::None, 0,0,0,12,true});
    add({"bat_wing",      "Bat Wing",         "Leathery wing.",              IC::Material, CT::None, 0,0,0,12,true});
    add({"wolf_pelt",     "Wolf Pelt",        "Rugged fur.",                 IC::Material, CT::None, 0,0,0,25,true});
    add({"wolf_fang",     "Wolf Fang",        "Sharp fang from a wolf.",     IC::Material, CT::None, 0,0,0,18,true});
    add({"bone_fragment", "Bone Fragment",    "Piece of ancient bone.",      IC::Material, CT::None, 0,0,0,22,true});
    add({"iron_ore",      "Iron Ore",         "Raw iron ore.",               IC::Material, CT::None, 0,0,0,30,true});
    // Materials — elemental
    add({"fire_essence",  "Fire Essence",     "Burning core.",               IC::Material, CT::None, 0,0,0,80,true});
    add({"water_essence", "Water Essence",    "Crystallized water magic.",   IC::Material, CT::None, 0,0,0,80,true});
    add({"earth_essence", "Earth Essence",    "Condensed earth energy.",     IC::Material, CT::None, 0,0,0,80,true});
    add({"wind_essence",  "Wind Essence",     "Captured wind spirit.",       IC::Material, CT::None, 0,0,0,80,true});
    add({"ember_stone",   "Ember Stone",      "Glows with inner fire.",      IC::Material, CT::None, 0,0,0,60,true});
    add({"aqua_crystal",  "Aqua Crystal",     "Filled with water magic.",    IC::Material, CT::None, 0,0,0,60,true});
    add({"stone_core",    "Stone Core",       "Heart of a stone golem.",     IC::Material, CT::None, 0,0,0,70,true});
    add({"gale_feather",  "Gale Feather",     "Floats on the wind.",         IC::Material, CT::None, 0,0,0,55,true});
    add({"orc_tusk",      "Orc Tusk",         "Massive tusk.",               IC::Material, CT::None, 0,0,0,65,true});
    // Materials — boss drops
    add({"dragon_scale",  "Dragon Scale",     "Impenetrable scale.",         IC::Material, CT::None, 0,0,0,300,true});
    add({"dragon_fang",   "Dragon Fang",      "Massive fang.",               IC::Material, CT::None, 0,0,0,250,true});
    add({"dark_crystal",  "Dark Crystal",     "Radiates dark energy.",       IC::Material, CT::None, 0,0,0,280,true});
    add({"kraken_tentacle","Kraken Tentacle", "Severed tentacle.",           IC::Material, CT::None, 0,0,0,280,true});
    add({"deep_sea_pearl","Deep Sea Pearl",   "Beauty and power.",           IC::Material, CT::None, 0,0,0,350,true});
    add({"titan_heart",   "Titan Heart",      "Core of an earth titan.",     IC::Material, CT::None, 0,0,0,400,true});
    add({"adamant_ore",   "Adamant Ore",      "Hardest ore known.",          IC::Material, CT::None, 0,0,0,200,true});
    add({"legendary_ore", "Legendary Ore",    "Mysterious ore.",             IC::Material, CT::None, 0,0,0,500,true});
}

void ItemRegistry::initShopStock() {
    shopStock_ = {
        {"small_potion",-1},{"medium_potion",-1},{"large_potion",-1},
        {"full_restore",5},{"atk_elixir",-1},{"def_elixir",-1},
        {"power_surge",3},{"iron_skin",3},{"antidote",-1}
    };
}