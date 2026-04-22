#include "weapons.hpp"

WeaponRegistry::WeaponRegistry() { load(); initShopStock(); }

const WeaponTemplate* WeaponRegistry::getByID(const std::string& id) const {
    auto it = weapons_.find(id);
    return it == weapons_.end() ? nullptr : &it->second;
}

std::vector<ShopWeaponEntry> WeaponRegistry::getShopWeapons() const {
    static const std::vector<std::string> order = {
        "iron_sword","steel_sword","fire_blade","aqua_saber",
        "earth_cleaver","wind_cutter","iron_axe","wooden_staff",
        "fire_staff","ice_staff","iron_dagger","assassin_blade",
        "hunting_bow","gale_bow"
    };
    std::vector<ShopWeaponEntry> result;
    for (auto& id : order) {
        auto it = weapons_.find(id);
        if (it != weapons_.end() && it->second.buyPrice > 0) {
            int stk = 0;
            auto sit = shopStock_.find(id);
            if (sit != shopStock_.end()) stk = sit->second;
            result.push_back({&it->second, stk});
        }
    }
    return result;
}

int WeaponRegistry::getStock(const std::string& id) const {
    auto it = shopStock_.find(id);
    return it == shopStock_.end() ? 0 : it->second;
}

bool WeaponRegistry::decrementStock(const std::string& id) {
    auto it = shopStock_.find(id);
    if (it == shopStock_.end()) return false;
    if (it->second == -1) return true;  // unlimited
    if (it->second > 0) { --it->second; return true; }
    return false;
}

void WeaponRegistry::load() {
    auto add = [&](WeaponTemplate t){ weapons_[t.id] = t; };
    // Starter
    add({"fists",         "Fists",          "Your bare hands.",                WeaponType::Fist,   Element::None,   5,   0,   0, 1});
    add({"rusty_sword",   "Rusty Sword",    "Old, covered in rust.",           WeaponType::Sword,  Element::None,   8,   0,  15, 1});
    // Swords
    add({"iron_sword",    "Iron Sword",     "Sturdy iron blade.",              WeaponType::Sword,  Element::None,  15, 100,  50, 5});
    add({"steel_sword",   "Steel Sword",    "Forged from high-quality steel.", WeaponType::Sword,  Element::None,  22, 250, 125, 5});
    add({"fire_blade",    "Fire Blade",     "Sword imbued with flames.",       WeaponType::Sword,  Element::Fire,  20, 300, 150, 5});
    add({"aqua_saber",    "Aqua Saber",     "Blade of crystallized water.",    WeaponType::Sword,  Element::Water, 20, 300, 150, 5});
    add({"earth_cleaver", "Earth Cleaver",  "Heavy blade infused with earth.", WeaponType::Sword,  Element::Earth, 20, 300, 150, 5});
    add({"wind_cutter",   "Wind Cutter",    "Swift blade that cuts like wind.",WeaponType::Sword,  Element::Wind,  18, 280, 140, 5});
    // Axes
    add({"iron_axe",      "Iron Axe",       "A heavy axe.",                   WeaponType::Axe,    Element::None,  20, 150,  75, 5});
    add({"battle_axe",    "Battle Axe",     "A warrior's axe.",               WeaponType::Axe,    Element::None,  28,   0, 100, 5});
    // Staves
    add({"wooden_staff",  "Wooden Staff",   "A simple staff.",                 WeaponType::Staff,  Element::None,  10,  80,  40, 3});
    add({"fire_staff",    "Fire Staff",     "Channels the power of flames.",   WeaponType::Staff,  Element::Fire,  18, 280, 140, 5});
    add({"ice_staff",     "Ice Staff",      "Frozen magic flows through it.",  WeaponType::Staff,  Element::Water, 18, 280, 140, 5});
    // Daggers
    add({"iron_dagger",   "Iron Dagger",    "Quick and deadly.",               WeaponType::Dagger, Element::None,  12,  80,  40, 5});
    add({"assassin_blade","Assassin Blade", "A shadowy dagger.",               WeaponType::Dagger, Element::Wind,  16, 220, 110, 5});
    // Bows
    add({"hunting_bow",   "Hunting Bow",    "Reliable bow for combat.",        WeaponType::Bow,    Element::None,  14, 120,  60, 5});
    add({"gale_bow",      "Gale Bow",       "Arrows fly swift as the wind.",   WeaponType::Bow,    Element::Wind,  18, 260, 130, 5});
    // Legendary
    add({"dragon_slayer", "Dragon Slayer",  "Forged from dragon scales.",      WeaponType::Sword,  Element::Fire,  45,   0, 500, 5});
    add({"titan_hammer",  "Titan's Hammer", "Weapon of the earth titans.",     WeaponType::Axe,    Element::Earth, 50,   0, 600, 5});
}

void WeaponRegistry::initShopStock() {
    shopStock_ = {
        {"iron_sword",-1},{"steel_sword",-1},{"fire_blade",1},{"aqua_saber",1},
        {"earth_cleaver",1},{"wind_cutter",1},{"iron_axe",-1},{"wooden_staff",-1},
        {"fire_staff",1},{"ice_staff",1},{"iron_dagger",-1},{"assassin_blade",1},
        {"hunting_bow",-1},{"gale_bow",1}
    };
}