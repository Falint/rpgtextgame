#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "../domain/inventory.hpp"

struct ItemTemplate {
    std::string    id;
    std::string    name;
    std::string    description;
    ItemCategory   category       = ItemCategory::Consumable;
    ConsumableType consumableType = ConsumableType::None;
    int            value          = 0;
    int            duration       = 0;
    int            buyPrice       = 0;
    int            sellPrice      = 0;
    bool           stackable      = true;

    std::shared_ptr<Item> toItem() const {
        auto i = std::make_shared<Item>();
        i->id             = id;
        i->name           = name;
        i->description    = description;
        i->category       = category;
        i->consumableType = consumableType;
        i->value          = value;
        i->duration       = duration;
        i->buyPrice       = buyPrice;
        i->sellPrice      = sellPrice;
        i->stackable      = stackable;
        return i;
    }
};

struct ShopItemEntry {
    const ItemTemplate* item  = nullptr;
    int stock = -1;
    bool isSoldOut()   const { return stock == 0; }
    bool isUnlimited() const { return stock == -1; }
};

class ItemRegistry {
public:
    ItemRegistry();

    const ItemTemplate* getByID(const std::string& id) const;
    std::vector<ShopItemEntry> getShopItems() const;
    int  getStock(const std::string& id) const;
    bool decrementStock(const std::string& id, int amount = 1);

private:
    void load();
    void initShopStock();
    std::map<std::string, ItemTemplate> items_;
    std::map<std::string, int>          shopStock_;
};