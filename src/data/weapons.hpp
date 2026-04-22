#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "../domain/weapon.hpp"

struct WeaponTemplate {
    std::string id;
    std::string name;
    std::string description;
    WeaponType  type       = WeaponType::Fist;
    Element     element    = Element::None;
    int         baseDamage = 0;
    int         buyPrice   = 0;
    int         sellPrice  = 0;
    int         maxTier    = 5;

    std::shared_ptr<Weapon> toWeapon() const {
        auto w = std::make_shared<Weapon>();
        w->id          = id;
        w->name        = name;
        w->description = description;
        w->type        = type;
        w->element     = element;
        w->baseDamage  = baseDamage;
        w->tier        = 1;
        w->maxTier     = maxTier;
        return w;
    }
};

struct ShopWeaponEntry {
    const WeaponTemplate* weapon = nullptr;
    int stock = -1;   // -1 = unlimited, 0 = sold out, >0 = limited
    bool isSoldOut()   const { return stock == 0; }
    bool isUnlimited() const { return stock == -1; }
};

class WeaponRegistry {
public:
    WeaponRegistry();

    const WeaponTemplate* getByID(const std::string& id) const;
    std::vector<ShopWeaponEntry> getShopWeapons() const;
    int  getStock(const std::string& id) const;
    bool decrementStock(const std::string& id);

private:
    void load();
    void initShopStock();
    std::map<std::string, WeaponTemplate> weapons_;
    std::map<std::string, int>            shopStock_;
};