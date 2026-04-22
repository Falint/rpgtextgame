#include "upgrades.hpp"

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
    auto add = [&](UpgradeRecipe r){ recipes_[r.weaponID].push_back(r); };

    // Iron Sword
    add({"iron_sword",1,2, 80,{{"goblin_fang",3},{"slime_gel",2}},3});
    add({"iron_sword",2,3,150,{{"goblin_fang",5},{"wolf_fang",3}},4});
    add({"iron_sword",3,4,300,{{"orc_tusk",3},{"bone_fragment",5}},5});
    add({"iron_sword",4,5,500,{{"legendary_ore",1},{"orc_tusk",5}},8});
    // Fire Blade
    add({"fire_blade",1,2,120,{{"fire_essence",2},{"ember_stone",1}},4});
    add({"fire_blade",2,3,250,{{"fire_essence",4},{"ember_stone",3}},5});
    add({"fire_blade",3,4,450,{{"fire_essence",6},{"dragon_fang",1}},7});
    add({"fire_blade",4,5,800,{{"dragon_scale",2},{"legendary_ore",1}},10});
    // Aqua Saber
    add({"aqua_saber",1,2,120,{{"water_essence",2},{"aqua_crystal",1}},4});
    add({"aqua_saber",2,3,250,{{"water_essence",4},{"aqua_crystal",3}},5});
    add({"aqua_saber",3,4,450,{{"water_essence",6},{"deep_sea_pearl",1}},7});
    add({"aqua_saber",4,5,800,{{"kraken_tentacle",2},{"legendary_ore",1}},10});
    // Earth Cleaver
    add({"earth_cleaver",1,2,120,{{"earth_essence",2},{"stone_core",1}},4});
    add({"earth_cleaver",2,3,250,{{"earth_essence",4},{"stone_core",2}},5});
    add({"earth_cleaver",3,4,450,{{"earth_essence",6},{"adamant_ore",2}},7});
    add({"earth_cleaver",4,5,800,{{"titan_heart",1},{"legendary_ore",1}},10});
    // Wind Cutter
    add({"wind_cutter",1,2,100,{{"wind_essence",2},{"gale_feather",2}},3});
    add({"wind_cutter",2,3,220,{{"wind_essence",4},{"gale_feather",4}},4});
    add({"wind_cutter",3,4,400,{{"wind_essence",6},{"bat_wing",10}},6});
    add({"wind_cutter",4,5,700,{{"gale_feather",10},{"legendary_ore",1}},9});
    // Iron Axe
    add({"iron_axe",1,2,100,{{"wolf_pelt",3},{"bone_fragment",3}},4});
    add({"iron_axe",2,3,200,{{"orc_tusk",2},{"wolf_fang",5}},5});
    add({"iron_axe",3,4,400,{{"orc_tusk",5},{"stone_core",2}},7});
    add({"iron_axe",4,5,700,{{"adamant_ore",3},{"legendary_ore",1}},10});
}