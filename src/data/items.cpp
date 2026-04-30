#include "items.hpp"
#include "../db/data_loader.hpp"

ItemRegistry::ItemRegistry() { load(); initShopStock(); }

const ItemTemplate* ItemRegistry::getByID(const std::string& id) const {
    auto it = items_.find(id);
    return it == items_.end() ? nullptr : &it->second;
}

std::vector<ShopItemEntry> ItemRegistry::getShopItems() const {
    std::vector<ShopItemEntry> result;
    for (const auto& id : shopOrder_) {
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
    for (auto& t : DataLoader::loadItems()) {
        items_[t.id] = std::move(t);
    }
}

void ItemRegistry::initShopStock() {
    for (auto& [id, stock] : DataLoader::loadItemShop()) {
        shopOrder_.push_back(id);
        shopStock_[id] = stock;
    }
}