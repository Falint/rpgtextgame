#include "weapons.hpp"
#include "../db/data_loader.hpp"

WeaponRegistry::WeaponRegistry() { load(); initShopStock(); }

const WeaponTemplate* WeaponRegistry::getByID(const std::string& id) const {
    auto it = weapons_.find(id);
    return it == weapons_.end() ? nullptr : &it->second;
}

std::vector<ShopWeaponEntry> WeaponRegistry::getShopWeapons() const {
    std::vector<ShopWeaponEntry> result;
    for (const auto& id : shopOrder_) {
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
    for (auto& t : DataLoader::loadWeapons()) {
        weapons_[t.id] = std::move(t);
    }
}

void WeaponRegistry::initShopStock() {
    for (auto& [id, stock] : DataLoader::loadWeaponShop()) {
        shopOrder_.push_back(id);
        shopStock_[id] = stock;
    }
}