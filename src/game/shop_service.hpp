#pragma once
#include <string>
#include <tuple>
#include "../domain/player.hpp"
#include "../domain/errors.hpp"
#include "../data/weapons.hpp"
#include "../data/items.hpp"

static constexpr double DEFAULT_SELL_RATIO = 0.5;

class ShopService {
public:
    ShopService(Player* player, WeaponRegistry* weapons, ItemRegistry* items);

    ResultCode buyWeapon(const std::string& weaponID);
    ResultCode buyItem(const std::string& itemID);
    // Returns gold earned, ResultCode::Success on success.
    std::pair<int, ResultCode> sellItem(int slotIndex);

    std::vector<ShopWeaponEntry> getShopWeapons() const;
    std::vector<ShopItemEntry>   getShopItems()   const;

private:
    Player*         player_;
    WeaponRegistry* weaponReg_;
    ItemRegistry*   itemReg_;
    double          sellRatio_ = DEFAULT_SELL_RATIO;
};
