#pragma once
#include <memory>
#include <vector>
#include "../domain/player.hpp"
#include "../data/monsters.hpp"
#include "../data/weapons.hpp"
#include "../data/items.hpp"
#include "../data/upgrades.hpp"
#include "../game/battle_service.hpp"
#include "../game/shop_service.hpp"
#include "../game/item_service.hpp"
#include "renderer.hpp"
#include "screens.hpp"

// Menu item IDs
static constexpr const char* MENU_BATTLE_NORMAL = "battle_normal";
static constexpr const char* MENU_BATTLE_ELITE  = "battle_elite";
static constexpr const char* MENU_BATTLE_BOSS   = "battle_boss";
static constexpr const char* MENU_SHOP          = "shop";
static constexpr const char* MENU_UPGRADE       = "upgrade";
static constexpr const char* MENU_EQUIP         = "equip";
static constexpr const char* MENU_INVENTORY     = "inventory";
static constexpr const char* MENU_STATUS        = "status";
static constexpr const char* MENU_EXIT          = "exit";

class App {
public:
    App(std::shared_ptr<Player>  player,
        MonsterRegistry*         monsters,
        WeaponRegistry*          weapons,
        ItemRegistry*            items,
        UpgradeRegistry*         upgrades,
        BattleService*           battle,
        ShopService*             shop,
        ItemService*             itemSvc);

    // Blocking main loop.
    void run();

private:
    void handleMenuSelect();
    void resize();
    void draw();

    std::shared_ptr<Player> player_;
    MonsterRegistry*        monsters_;
    WeaponRegistry*         weapons_;
    ItemRegistry*           items_;
    UpgradeRegistry*        upgrades_;
    BattleService*          battle_;
    ShopService*            shop_;
    ItemService*            itemSvc_;

    Renderer renderer_;
    std::unique_ptr<Screen> activeScreen_;

    // Panel focus: 0 = menu, 1 = game panel
    int  focus_        = 0;
    int  menuCursor_   = 0;
    bool running_      = true;
    int  cols_         = 120;
    int  rows_         = 30;

    std::vector<MenuItem> menuItems_;
};
