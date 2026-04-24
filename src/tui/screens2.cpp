// screens2.cpp — ShopScreen
#include "screens.hpp"
#include <sstream>
#include <algorithm>

// ═══════════════════════════════════════════════════════════════════════════════
// ShopScreen
// ═══════════════════════════════════════════════════════════════════════════════
ShopScreen::ShopScreen(ShopService* svc, WeaponRegistry* w, ItemRegistry* it, Player* p)
    : svc_(svc), weapons_(w), items_(it), player_(p) {}

bool ShopScreen::update(Key key, char ch) {
    if (key==Key::Up   ||(key==Key::Char&&ch=='k')) { if(cursor_>0)--cursor_; return true; }
    if (key==Key::Down ||(key==Key::Char&&ch=='j')) { ++cursor_; return true; }
    if (key==Key::Enter) { handleSelect(); return true; }
    if (key==Key::Escape||key==Key::Backspace) {
        if (mode_!=Mode::Main) { mode_=Mode::Main; cursor_=0; msg_=""; return true; }
        return false;
    }
    if (key>=Key::K1 && key<=Key::K9) {
        cursor_ = static_cast<int>(key) - static_cast<int>(Key::K1);
        handleSelect();
    }
    return true;
}

void ShopScreen::handleSelect() {
    if (mode_==Mode::Main) {
        switch(cursor_) {
        case 0: mode_=Mode::BuyWeapons; cursor_=0; msg_=""; break;
        case 1: mode_=Mode::BuyItems;   cursor_=0; msg_=""; break;
        case 2: mode_=Mode::Sell;       cursor_=0; msg_=""; break;
        default: break;
        }
        return;
    }
    if (mode_==Mode::BuyWeapons) { handleBuyWeapon(); return; }
    if (mode_==Mode::BuyItems)   { handleBuyItem();   return; }
    if (mode_==Mode::Sell)       { handleSell();      return; }
}

void ShopScreen::handleBuyWeapon() {
    auto entries = svc_->getShopWeapons();
    tui::clamp(cursor_,(int)entries.size());
    if (cursor_<0||cursor_>=(int)entries.size()) return;
    auto& e = entries[cursor_];
    if (e.isSoldOut()) { msg_="SOLD OUT!"; msgStyle_="error"; return; }
    auto rc = svc_->buyWeapon(e.weapon->id);
    if (rc==ResultCode::Success)         { msg_="Purchased "+e.weapon->name+"!"; msgStyle_="success"; }
    else if (rc==ResultCode::NotEnoughGold) { msg_="Not enough gold!";  msgStyle_="error"; }
    else if (rc==ResultCode::InventoryFull) { msg_="Inventory full!";   msgStyle_="error"; }
    else                                    { msg_="Purchase failed!";  msgStyle_="error"; }
}

void ShopScreen::handleBuyItem() {
    auto entries = svc_->getShopItems();
    tui::clamp(cursor_,(int)entries.size());
    if (cursor_<0||cursor_>=(int)entries.size()) return;
    auto& e = entries[cursor_];
    if (e.isSoldOut()) { msg_="SOLD OUT!"; msgStyle_="error"; return; }
    auto rc = svc_->buyItem(e.item->id);
    if (rc==ResultCode::Success)            { msg_="Purchased "+e.item->name+"!"; msgStyle_="success"; }
    else if (rc==ResultCode::NotEnoughGold) { msg_="Not enough gold!";  msgStyle_="error"; }
    else if (rc==ResultCode::InventoryFull) { msg_="Inventory full!";   msgStyle_="error"; }
    else                                    { msg_="Purchase failed!";  msgStyle_="error"; }
}

void ShopScreen::handleSell() {
    auto slots = player_->inventory->getOccupiedSlots();
    tui::clamp(cursor_,(int)slots.size());
    if (cursor_<0||cursor_>=(int)slots.size()) return;
    auto result = svc_->sellItem(slots[cursor_]);
    int goldEarned = result.first;
    ResultCode rc  = result.second;
    if (rc==ResultCode::Success) { msg_="Sold for "+std::to_string(goldEarned)+" G!"; msgStyle_="success"; }
    else                         { msg_="Cannot sell!"; msgStyle_="error"; }
}

std::string ShopScreen::render() const {
    std::string out;
    out += tui::header("=== SHOP ===") + "\n\n";
    out += std::string(CLR_DIM)+"Your Gold: "+ANSI_RESET
         + Terminal::gold(std::to_string(player_->gold)+" G") + "\n";

    if (mode_==Mode::Main) {
        out += "\n" + tui::dim("--- Shop Menu ---") + "\n";
        const char* opts[]={"1. Buy Weapons","2. Buy Items","3. Sell Items","4. Exit Shop"};
        for (int i=0; i<4; ++i)
            out += (i==cursor_ ? Terminal::selected(opts[i]) : Terminal::normal(opts[i])) + "\n";
        out += "Choice: ";

    } else if (mode_==Mode::BuyWeapons) {
        out += "\n" + tui::header("=== WEAPONS ===") + "\n\n";
        auto entries = svc_->getShopWeapons();
        int cur = tui::clamped(cursor_, (int)entries.size());
        for (int i=0; i<(int)entries.size(); ++i) {
            auto& e = entries[i];
            std::string line = std::to_string(i+1)+". ";
            if (e.isSoldOut()) {
                line += Terminal::muted("[SOLD] "+e.weapon->name);
            } else {
                const char* clr = CLR_TEXT;
                switch (e.weapon->element) {
                case Element::Fire:  clr=CLR_FIRE;  break;
                case Element::Water: clr=CLR_WATER; break;
                case Element::Earth: clr=CLR_EARTH; break;
                case Element::Wind:  clr=CLR_WIND;  break;
                default: break;
                }
                line += std::string(clr)+e.weapon->name+ANSI_RESET;
                const char* pc = player_->gold>=e.weapon->buyPrice ? CLR_GOLD : CLR_DANGER;
                line += " - "+std::string(pc)+std::to_string(e.weapon->buyPrice)+" G"+ANSI_RESET;
                line += std::string(CLR_DIM)+" | DMG:"+ANSI_RESET+std::to_string(e.weapon->baseDamage);
                if (e.weapon->element!=Element::None)
                    line += std::string(" (")+elementStr(e.weapon->element)+")";
                if (!e.isUnlimited())
                    line += " [Stock:"+std::to_string(e.stock)+"]";
            }
            out += (i==cur ? Terminal::selected(line) : line) + "\n";
        }
        out += "\n" + Terminal::muted("[Enter] Buy  [Esc] Back");

    } else if (mode_==Mode::BuyItems) {
        out += "\n" + tui::header("=== ITEMS ===") + "\n\n";
        auto entries = svc_->getShopItems();
        int cur = tui::clamped(cursor_, (int)entries.size());
        for (int i=0; i<(int)entries.size(); ++i) {
            auto& e = entries[i];
            std::string line = std::to_string(i+1)+". ";
            if (e.isSoldOut()) {
                line += Terminal::muted("[SOLD] "+e.item->name);
            } else {
                line += std::string(CLR_TEXT)+e.item->name+ANSI_RESET;
                const char* pc = player_->gold>=e.item->buyPrice ? CLR_GOLD : CLR_DANGER;
                line += " - "+std::string(pc)+std::to_string(e.item->buyPrice)+" G"+ANSI_RESET;
                line += " "+std::string(CLR_DIM)+"("+e.item->description+")"+ANSI_RESET;
                if (!e.isUnlimited())
                    line += " [Stock:"+std::to_string(e.stock)+"]";
            }
            out += (i==cur ? Terminal::selected(line) : line) + "\n";
        }
        out += "\n" + Terminal::muted("[Enter] Buy  [Esc] Back");

    } else if (mode_==Mode::Sell) {
        out += "\n" + tui::header("=== SELL ITEMS ===") + "\n\n";
        auto slots = player_->inventory->getOccupiedSlots();
        int cur = tui::clamped(cursor_, (int)slots.size());
        if (slots.empty()) {
            out += Terminal::muted("  No items to sell");
        } else {
            for (int i=0; i<(int)slots.size(); ++i) {
                auto* s = player_->inventory->getSlot(slots[i]);
                std::string line = std::to_string(i+1)+". ";
                if (s->type==SlotType::WeaponSlot && s->weapon) {
                    // Look up sell price from weapon registry
                    auto* tmpl = weapons_->getByID(s->weapon->id);
                    int sp = tmpl ? tmpl->sellPrice/2 : 0;
                    line += s->weapon->displayName()
                          + " - " + Terminal::gold("Sell: "+std::to_string(sp)+" G");
                } else if (s->type==SlotType::ItemSlot && s->item) {
                    line += s->item->name+" (x"+std::to_string(s->quantity)+") - "
                          + Terminal::gold("Sell: "+std::to_string(s->item->sellPrice)+" G");
                }
                out += (i==cur ? Terminal::selected(line) : line) + "\n";
            }
        }
        out += "\n" + Terminal::muted("[Enter] Sell  [Esc] Back");
    }

    if (!msg_.empty())
        out += "\n" + (msgStyle_=="success" ? Terminal::success(msg_) : Terminal::error(msg_));
    return out;
}
