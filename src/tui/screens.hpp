#pragma once
#include <string>
#include "../tui/terminal.hpp"
#include "../domain/player.hpp"
#include "../game/battle_service.hpp"
#include "../game/shop_service.hpp"
#include "../game/item_service.hpp"
#include "../data/upgrades.hpp"
#include "../data/weapons.hpp"
#include "../data/items.hpp"

// ─── Shared UI Helpers ─────────────────────────────────────────────────────────
namespace tui {
    inline void clamp(int& cur, int max) {
        if (max <= 0) { cur = 0; return; }
        if (cur < 0)    cur = 0;
        if (cur >= max) cur = max - 1;
    }
    inline int clamped(int v, int mx) {
        if (mx <= 0) return 0;
        if (v < 0)   return 0;
        if (v >= mx) return mx - 1;
        return v;
    }
    inline std::string header(const std::string& s) {
        return std::string(CLR_ACCENT) + ANSI_BOLD + s + ANSI_RESET;
    }
    inline std::string dim(const std::string& s) {
        return std::string(CLR_SECONDARY) + s + ANSI_RESET;
    }
}

// ─── Screen interface ─────────────────────────────────────────────────────────
class Screen {
public:
    virtual ~Screen() = default;
    // Returns false to signal "exit this screen".
    virtual bool update(Key key, char ch) = 0;
    virtual std::string render() const = 0;
    virtual std::string title() const = 0;
};

// ─── Battle screen ────────────────────────────────────────────────────────────
class BattleScreen : public Screen {
public:
    BattleScreen(BattleService* svc, Player* player);
    bool update(Key key, char ch) override;
    std::string render() const override;
    std::string title() const override { return "BATTLE"; }
private:
    enum class Mode { Main, ItemSelect, Result };
    void handleSelect();
    void handleUseItem();
    int  findConsumableSlot(int n) const;
    BattleService* svc_;
    Player*        player_;
    Mode           mode_   = Mode::Main;
    int            cursor_ = 0;
};

// ─── Shop screen ──────────────────────────────────────────────────────────────
class ShopScreen : public Screen {
public:
    ShopScreen(ShopService* svc, WeaponRegistry* weapons,
               ItemRegistry* items, Player* player);
    bool update(Key key, char ch) override;
    std::string render() const override;
    std::string title() const override { return "SHOP"; }
private:
    enum class Mode { Main, BuyWeapons, BuyItems, Sell };
    void handleSelect();
    void handleBuyWeapon();
    void handleBuyItem();
    void handleSell();
    ShopService*    svc_;
    WeaponRegistry* weapons_;
    ItemRegistry*   items_;
    Player*         player_;
    Mode            mode_    = Mode::Main;
    int             cursor_  = 0;
    mutable std::string msg_;
    mutable std::string msgStyle_;
};

// ─── Inventory screen ─────────────────────────────────────────────────────────
class InventoryScreen : public Screen {
public:
    InventoryScreen(Player* player, ItemService* svc);
    bool update(Key key, char ch) override;
    std::string render() const override;
    std::string title() const override { return "INVENTORY"; }
private:
    enum class Mode { Main, Weapons, Consumables, Materials, UseItem };
    void handleSelect();
    void handleUseItem();
    int  findConsumableSlot(int n) const;
    Player*      player_;
    ItemService* svc_;
    Mode         mode_    = Mode::Main;
    int          cursor_  = 0;
    mutable std::string msg_, msgStyle_;
};

// ─── Equip screen ─────────────────────────────────────────────────────────────
class EquipScreen : public Screen {
public:
    explicit EquipScreen(Player* player);
    bool update(Key key, char ch) override;
    std::string render() const override;
    std::string title() const override { return "EQUIP WEAPON"; }
private:
    struct Entry { std::shared_ptr<Weapon> weapon; int slotIdx; };
    void handleEquip();
    std::vector<Entry> getEquippable() const;
    Player* player_;
    int     cursor_ = 0;
    mutable std::string msg_, msgStyle_;
};

// ─── Upgrade screen ───────────────────────────────────────────────────────────
class UpgradeScreen : public Screen {
public:
    UpgradeScreen(Player* player, WeaponRegistry* weapons,
                  ItemRegistry* items, UpgradeRegistry* upgrades);
    bool update(Key key, char ch) override;
    std::string render() const override;
    std::string title() const override { return "WEAPON UPGRADE"; }
private:
    enum class Mode { Main, InventorySelect, Details };
    struct Entry { Weapon* weapon; int slotIdx; bool equipped; };
    std::vector<Entry> getUpgradeable() const;
    bool checkMaterials(const UpgradeRecipe* r) const;
    void consumeMaterials(const UpgradeRecipe* r);
    void doUpgradeEquipped();
    void doUpgradeInventory();
    Player*          player_;
    WeaponRegistry*  weapons_;
    ItemRegistry*    items_;
    UpgradeRegistry* upgrades_;
    Mode             mode_    = Mode::Main;
    int              cursor_  = 0;
    mutable std::string msg_, msgStyle_;
};

// ─── Status screen ────────────────────────────────────────────────────────────
class StatusScreen : public Screen {
public:
    explicit StatusScreen(Player* player);
    bool update(Key key, char ch) override;
    std::string render() const override;
    std::string title() const override { return "STATUS"; }
private:
    Player* player_;
};
