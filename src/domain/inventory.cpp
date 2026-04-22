#include "inventory.hpp"
#include <algorithm>

// ─── addWeapon ───────────────────────────────────────────────────────────────
ResultCode Inventory::addWeapon(std::shared_ptr<Weapon> w) {
    for (auto& s : slots_) {
        if (s.type == SlotType::Empty) {
            s.type     = SlotType::WeaponSlot;
            s.weapon   = w;
            s.quantity = 1;
            return ResultCode::Success;
        }
    }
    return ResultCode::InventoryFull;
}

// ─── addItem ─────────────────────────────────────────────────────────────────
ResultCode Inventory::addItem(std::shared_ptr<Item> item, int qty) {
    if (item->stackable) {
        for (auto& s : slots_) {
            if (s.type == SlotType::ItemSlot && s.item && s.item->id == item->id) {
                s.quantity += qty;
                if (s.quantity > MAX_ITEM_STACK_SIZE) s.quantity = MAX_ITEM_STACK_SIZE;
                return ResultCode::Success;
            }
        }
    }
    for (auto& s : slots_) {
        if (s.type == SlotType::Empty) {
            s.type     = SlotType::ItemSlot;
            s.item     = item;
            s.quantity = qty;
            return ResultCode::Success;
        }
    }
    return ResultCode::InventoryFull;
}

// ─── removeItem ──────────────────────────────────────────────────────────────
ResultCode Inventory::removeItem(int idx, int qty) {
    if (idx < 0 || idx >= static_cast<int>(slots_.size())) return ResultCode::InvalidSlot;
    auto& s = slots_[idx];
    if (s.type == SlotType::Empty) return ResultCode::ItemNotFound;
    s.quantity -= qty;
    if (s.quantity <= 0) s = InventorySlot{};  // clear
    return ResultCode::Success;
}

// ─── removeWeapon ────────────────────────────────────────────────────────────
std::shared_ptr<Weapon> Inventory::removeWeapon(int idx, ResultCode& out) {
    if (idx < 0 || idx >= static_cast<int>(slots_.size())) { out = ResultCode::InvalidSlot; return nullptr; }
    auto& s = slots_[idx];
    if (s.type != SlotType::WeaponSlot) { out = ResultCode::ItemNotFound; return nullptr; }
    auto w = s.weapon;
    s = InventorySlot{};
    out = ResultCode::Success;
    return w;
}

// ─── getSlot ─────────────────────────────────────────────────────────────────
InventorySlot* Inventory::getSlot(int idx) {
    if (idx < 0 || idx >= static_cast<int>(slots_.size())) return nullptr;
    return &slots_[idx];
}
const InventorySlot* Inventory::getSlot(int idx) const {
    if (idx < 0 || idx >= static_cast<int>(slots_.size())) return nullptr;
    return &slots_[idx];
}

// ─── usedSlots ───────────────────────────────────────────────────────────────
int Inventory::usedSlots() const {
    int c = 0;
    for (auto& s : slots_) if (s.type != SlotType::Empty) ++c;
    return c;
}

// ─── getWeapons ──────────────────────────────────────────────────────────────
std::vector<std::shared_ptr<Weapon>> Inventory::getWeapons() const {
    std::vector<std::shared_ptr<Weapon>> v;
    for (auto& s : slots_)
        if (s.type == SlotType::WeaponSlot && s.weapon) v.push_back(s.weapon);
    return v;
}

// ─── getItems ────────────────────────────────────────────────────────────────
std::vector<InventorySlot*> Inventory::getItems() {
    std::vector<InventorySlot*> v;
    for (auto& s : slots_)
        if (s.type == SlotType::ItemSlot) v.push_back(&s);
    return v;
}

// ─── getConsumables ──────────────────────────────────────────────────────────
std::vector<InventorySlot*> Inventory::getConsumables() {
    std::vector<InventorySlot*> v;
    for (auto& s : slots_)
        if (s.type == SlotType::ItemSlot && s.item &&
            s.item->category == ItemCategory::Consumable)
            v.push_back(&s);
    return v;
}

// ─── getMaterials ────────────────────────────────────────────────────────────
std::vector<MaterialInfo> Inventory::getMaterials() const {
    std::vector<MaterialInfo> v;
    for (auto& s : slots_)
        if (s.type == SlotType::ItemSlot && s.item &&
            s.item->category == ItemCategory::Material)
            v.push_back({s.item->name, s.item->id, s.quantity});
    return v;
}

// ─── getOccupiedSlots ────────────────────────────────────────────────────────
std::vector<int> Inventory::getOccupiedSlots() const {
    std::vector<int> v;
    for (int i = 0; i < static_cast<int>(slots_.size()); ++i)
        if (slots_[i].type != SlotType::Empty) v.push_back(i);
    return v;
}

// ─── countItem ───────────────────────────────────────────────────────────────
int Inventory::countItem(const std::string& id) const {
    for (auto& s : slots_)
        if (s.type == SlotType::ItemSlot && s.item && s.item->id == id)
            return s.quantity;
    return 0;
}

// ─── findItemSlot ────────────────────────────────────────────────────────────
int Inventory::findItemSlot(const std::string& id) const {
    for (int i = 0; i < static_cast<int>(slots_.size()); ++i)
        if (slots_[i].type == SlotType::ItemSlot && slots_[i].item &&
            slots_[i].item->id == id) return i;
    return -1;
}

// ─── removeItemByID ──────────────────────────────────────────────────────────
ResultCode Inventory::removeItemByID(const std::string& id, int qty) {
    int rem = qty;
    for (auto& s : slots_) {
        if (rem <= 0) break;
        if (s.type == SlotType::ItemSlot && s.item && s.item->id == id) {
            if (s.quantity <= rem) { rem -= s.quantity; s = InventorySlot{}; }
            else                  { s.quantity -= rem;  rem = 0; }
        }
    }
    return (rem > 0) ? ResultCode::ItemNotFound : ResultCode::Success;
}
