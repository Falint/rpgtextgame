#include "app.hpp"
#include <cstdio>
#include <string>

App::App(std::shared_ptr<Player> player,
         MonsterRegistry* monsters, WeaponRegistry* weapons,
         ItemRegistry* items, UpgradeRegistry* upgrades,
         BattleService* battle, ShopService* shop, ItemService* itemSvc)
    : player_(player), monsters_(monsters), weapons_(weapons),
      items_(items), upgrades_(upgrades), battle_(battle),
      shop_(shop), itemSvc_(itemSvc)
{
    menuItems_ = {
        {MENU_BATTLE_NORMAL, "Battle (Normal)", "1"},
        {MENU_BATTLE_ELITE,  "Battle (Elite)",  "2"},
        {MENU_BATTLE_BOSS,   "Battle (BOSS)",   "3"},
        {MENU_SHOP,          "Shop",            "4"},
        {MENU_UPGRADE,       "Upgrade Weapons", "5"},
        {MENU_EQUIP,         "Equip Weapon",    "6"},
        {MENU_INVENTORY,     "Inventory",       "7"},
        {MENU_STATUS,        "Status",          "8"},
        {MENU_EXIT,          "Exit",            "9"},
    };
}

void App::resize() {
    Terminal::getSize(cols_, rows_);
    renderer_.setSize(cols_, rows_);
}

void App::draw() {
    std::string menuContent = renderMenuPanel(menuItems_, menuCursor_, focus_ == 0, renderer_.menuWidth());
    std::string charContent = renderCharacterPanel(player_.get(), renderer_.charWidth());
    std::string gameContent;
    if (activeScreen_)
        gameContent = activeScreen_->render();
    else
        gameContent = "\n\n  Welcome to TextRPG!\n\n  Select an option from the menu.";
    renderer_.draw(menuContent, charContent, gameContent, focus_ == 1);
}

void App::run() {
    // Setup
    Terminal::enableANSI();
    Terminal::enableRawMode();
    printf(ANSI_ALT_SCREEN ANSI_HIDE_CURSOR);
    resize();

    while (running_) {
        draw();

        char ch = 0;
        Key key = Terminal::readKey(ch);

        // Global quit
        if (key == Key::CtrlC || key == Key::CtrlQ) break;

        // Resize check (no signal on Windows, just re-query each frame)
        int nc, nr;
        Terminal::getSize(nc, nr);
        if (nc != cols_ || nr != rows_) resize();

        if (focus_ == 1 && activeScreen_) {
            bool keep = activeScreen_->update(key, ch);
            if (!keep) {
                activeScreen_.reset();
                focus_ = 0;
            }
        } else {
            // Menu navigation
            if (key == Key::Up   || (key == Key::Char && ch == 'k')) {
                if (menuCursor_ > 0) --menuCursor_;
            } else if (key == Key::Down || (key == Key::Char && ch == 'j')) {
                if (menuCursor_ < (int)menuItems_.size() - 1) ++menuCursor_;
            } else if (key == Key::Enter) {
                handleMenuSelect();
            } else if (key >= Key::K1 && key <= Key::K9) {
                int idx = static_cast<int>(key) - static_cast<int>(Key::K1);
                if (idx < (int)menuItems_.size()) {
                    menuCursor_ = idx;
                    handleMenuSelect();
                }
            }
        }
    }

    // Cleanup
    printf(ANSI_SHOW_CURSOR ANSI_NORM_SCREEN);
    Terminal::disableRawMode();
}

void App::handleMenuSelect() {
    const auto& id = menuItems_[menuCursor_].id;
    if (id == MENU_EXIT) { running_ = false; return; }

    if (id == MENU_BATTLE_NORMAL) {
        battle_->startBattle(MonsterType::Normal);
        activeScreen_ = std::make_unique<BattleScreen>(battle_, player_.get());
    } else if (id == MENU_BATTLE_ELITE) {
        battle_->startBattle(MonsterType::Elite);
        activeScreen_ = std::make_unique<BattleScreen>(battle_, player_.get());
    } else if (id == MENU_BATTLE_BOSS) {
        battle_->startBattle(MonsterType::Boss);
        activeScreen_ = std::make_unique<BattleScreen>(battle_, player_.get());
    } else if (id == MENU_SHOP) {
        activeScreen_ = std::make_unique<ShopScreen>(shop_, weapons_, items_, player_.get());
    } else if (id == MENU_UPGRADE) {
        activeScreen_ = std::make_unique<UpgradeScreen>(player_.get(), weapons_, items_, upgrades_);
    } else if (id == MENU_EQUIP) {
        activeScreen_ = std::make_unique<EquipScreen>(player_.get());
    } else if (id == MENU_INVENTORY) {
        activeScreen_ = std::make_unique<InventoryScreen>(player_.get(), itemSvc_);
    } else if (id == MENU_STATUS) {
        activeScreen_ = std::make_unique<StatusScreen>(player_.get());
    }

    if (activeScreen_) focus_ = 1;
}
