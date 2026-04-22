#include "shop_service.hpp"

ShopService::ShopService(Player* player, WeaponRegistry* weapons, ItemRegistry* items)
    : player_(player), weaponReg_(weapons), itemReg_(items) {}

ResultCode ShopService::buyWeapon(const std::string& id) {
    auto* tmpl = weaponReg_->getByID(id);
    if (!tmpl) return ResultCode::ItemNotFound;
    if (player_->gold < tmpl->buyPrice) return ResultCode::NotEnoughGold;
    auto w = tmpl->toWeapon();
    auto rc = player_->inventory->addWeapon(w);
    if (rc != ResultCode::Success) return rc;
    player_->gold -= tmpl->buyPrice;
    return ResultCode::Success;
}

ResultCode ShopService::buyItem(const std::string& id) {
    auto* tmpl = itemReg_->getByID(id);
    if (!tmpl) return ResultCode::ItemNotFound;
    if (player_->gold < tmpl->buyPrice) return ResultCode::NotEnoughGold;
    auto item = tmpl->toItem();
    auto rc = player_->inventory->addItem(item, 1);
    if (rc != ResultCode::Success) return rc;
    player_->gold -= tmpl->buyPrice;
    return ResultCode::Success;
}

std::pair<int, ResultCode> ShopService::sellItem(int slotIndex) {
    auto* slot = player_->inventory->getSlot(slotIndex);
    if (!slot || slot->type == SlotType::Empty)
        return {0, ResultCode::InvalidSlot};

    int sellValue = 0;
    if (slot->type == SlotType::WeaponSlot && slot->weapon) {
        auto* tmpl = weaponReg_->getByID(slot->weapon->id);
        if (tmpl) sellValue = static_cast<int>(tmpl->sellPrice * sellRatio_);
    } else if (slot->type == SlotType::ItemSlot && slot->item) {
        sellValue = slot->item->sellPrice;
    }

    player_->inventory->removeItem(slotIndex, 1);
    player_->addGold(sellValue);
    return {sellValue, ResultCode::Success};
}

std::vector<ShopWeaponEntry> ShopService::getShopWeapons() const {
    return weaponReg_->getShopWeapons();
}

std::vector<ShopItemEntry> ShopService::getShopItems() const {
    return itemReg_->getShopItems();
}
