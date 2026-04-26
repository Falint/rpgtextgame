#pragma once
#include "errors.hpp"
#include "weapon.hpp"
#include <memory>
#include <string>
#include <vector>

// ─── Item enums ─────────────────────────────────────────────────────────────
enum class ItemCategory { Consumable = 0, Material };
enum class ConsumableType { None = 0, Heal, BuffAtk, BuffDef, FullRestore };

struct Item {
  std::string id;
  std::string name;
  std::string description;
  ItemCategory category = ItemCategory::Consumable;
  ConsumableType consumableType = ConsumableType::None;
  int value = 0;    // heal amount or buff %
  int duration = 0; // buff turns
  int buyPrice = 0;
  int sellPrice = 0;
  bool stackable = true;
};

// ─── Inventory slot ──────────────────────────────────────────────────────────
enum class SlotType { Empty = 0, WeaponSlot, ItemSlot };

struct InventorySlot {
  SlotType type = SlotType::Empty;
  std::shared_ptr<Weapon> weapon = nullptr;
  std::shared_ptr<Item> item = nullptr;
  int quantity = 0;
};

// ─── Material summary (for display) ─────────────────────────────────────────
struct MaterialInfo {
  std::string name;
  std::string id;
  int quantity = 0;
};

// ─── Constants ───────────────────────────────────────────────────────────────
inline constexpr int MAX_INVENTORY_SLOTS = 20;
inline constexpr int MAX_ITEM_STACK_SIZE = 99;

// ─── Inventory ───────────────────────────────────────────────────────────────
class Inventory {
public:
  explicit Inventory(int maxSlots = MAX_INVENTORY_SLOTS)
      : slots_(maxSlots), maxSlots_(maxSlots) {}

  ResultCode addWeapon(std::shared_ptr<Weapon> w);
  ResultCode addItem(std::shared_ptr<Item> item, int qty);
  ResultCode removeItem(int slotIndex, int qty);

  // Returns the weapon removed, nullptr on failure.
  std::shared_ptr<Weapon> removeWeapon(int slotIndex, ResultCode &out);

  InventorySlot *getSlot(int index);
  const InventorySlot *getSlot(int index) const;

  int usedSlots() const;
  int maxSlots() const { return maxSlots_; }

  std::vector<std::shared_ptr<Weapon>> getWeapons() const;
  std::vector<InventorySlot *> getItems();
  std::vector<InventorySlot *> getConsumables();
  std::vector<MaterialInfo> getMaterials() const;
  std::vector<int> getOccupiedSlots() const;

  int countItem(const std::string &itemID) const;
  int findItemSlot(const std::string &itemID) const;
  ResultCode removeItemByID(const std::string &itemID, int qty);

private:
  std::vector<InventorySlot> slots_;
  int maxSlots_;
};
