#include "screens.hpp"
#include <sstream>
#include <algorithm>
#include <string>

// ═══════════════════════════════════════════════════════════════════════════════
// BattleScreen
// ═══════════════════════════════════════════════════════════════════════════════
BattleScreen::BattleScreen(BattleService* svc, Player* player)
    : svc_(svc), player_(player) {}

bool BattleScreen::update(Key key, char ch) {
    if (mode_ == Mode::Result) {
        if (key == Key::Enter || key == Key::Escape) {
            svc_->endBattle(); return false;
        }
        return true;
    }
    switch (key) {
    case Key::Up:   case Key::Char: if (ch=='k'||key==Key::Up) { if(cursor_>0)--cursor_; } break;
    case Key::Down: ++cursor_; tui::clamp(cursor_, mode_==Mode::Main ? 3 : (int)player_->inventory->getConsumables().size()); break;
    case Key::Enter: handleSelect(); break;
    case Key::Escape:
        if (mode_ == Mode::ItemSelect) { mode_ = Mode::Main; cursor_ = 0; }
        break;
    case Key::K1: if(mode_==Mode::Main){cursor_=0;handleSelect();} break;
    case Key::K2: if(mode_==Mode::Main){cursor_=1;handleSelect();} break;
    case Key::K3: if(mode_==Mode::Main){cursor_=2;handleSelect();} break;
    default: break;
    }
    if (key == Key::Up && ch == 0) { if(cursor_>0)--cursor_; }
    return true;
}

void BattleScreen::handleSelect() {
    if (mode_ == Mode::Main) {
        switch (cursor_) {
        case 0: svc_->attack(); break;
        case 1: mode_ = Mode::ItemSelect; cursor_ = 0; return;
        case 2: svc_->tryEscape(); break;
        }
    } else if (mode_ == Mode::ItemSelect) {
        handleUseItem(); return;
    }
    if (svc_->isBattleOver()) mode_ = Mode::Result;
}

void BattleScreen::handleUseItem() {
    auto cons = player_->inventory->getConsumables();
    if (cons.empty()) { mode_ = Mode::Main; return; }
    tui::clamp(cursor_, (int)cons.size());
    int slotIdx = findConsumableSlot(cursor_);
    if (slotIdx != -1) {
        svc_->useItem(slotIdx);
        mode_ = Mode::Main;
        if (svc_->isBattleOver()) mode_ = Mode::Result;
    }
}

int BattleScreen::findConsumableSlot(int n) const {
    int count = 0;
    for (int i = 0; i < MAX_INVENTORY_SLOTS; ++i) {
        auto* s = player_->inventory->getSlot(i);
        if (s && s->type == SlotType::ItemSlot && s->item &&
            s->item->category == ItemCategory::Consumable) {
            if (count == n) return i;
            ++count;
        }
    }
    return -1;
}

std::string BattleScreen::render() const {
    std::string out;
    out += tui::header("=== BATTLE! ===") + "\n\n";

    auto* e = svc_->getEnemy();
    if (!e) return "No active battle.\n";

    // Enemy name
    std::string ename = e->name;
    if (e->type == MonsterType::Elite)
        ename = std::string(CLR_WARNING) + ANSI_BOLD + "[ELITE] " + ename + ANSI_RESET;
    else if (e->type == MonsterType::Boss)
        ename = std::string(CLR_DANGER)  + ANSI_BOLD + "[BOSS] "  + ename + ANSI_RESET;
    out += ename + "\n";

    // HP
    std::string hpStr = "HP: " + std::to_string(e->currentHP) + "/" + std::to_string(e->maxHP);
    out += Terminal::hpColor(e->currentHP, e->maxHP, hpStr);
    if (e->element != Element::None)
        out += std::string("  [") + elementStr(e->element) + "]";
    out += "\n" + tui::dim(std::string(30, '-')) + "\n";

    // Log (last 8 entries)
    auto& logs = svc_->getLogs();
    int start = std::max(0, (int)logs.size() - 8);
    for (int i = start; i < (int)logs.size(); ++i)
        out += "> " + logs[i] + "\n";
    if (logs.empty()) out += Terminal::muted("(Battle started...)") + "\n";

    out += "\n";

    if (mode_ == Mode::Result) {
        auto st = svc_->state();
        if (st == BattleState::Won)
            out += Terminal::success("VICTORY!") + "\nPress [Enter] to continue...";
        else if (st == BattleState::Lost)
            out += Terminal::error("DEFEAT...") + "\nPress [Enter] to continue...";
        else
            out += Terminal::muted("ESCAPED!") + "\nPress [Enter] to continue...";
    } else if (mode_ == Mode::ItemSelect) {
        out += tui::dim("--- Select Item ---") + "\n";
        auto cons = player_->inventory->getConsumables();
        if (cons.empty()) {
            out += Terminal::muted("  No items!") + "\n" + Terminal::muted("[Esc] Back");
        } else {
            for (int i = 0; i < (int)cons.size(); ++i) {
                std::string line = std::to_string(i+1) + ". " +
                    cons[i]->item->name + " x" + std::to_string(cons[i]->quantity);
                out += (i == cursor_ ? Terminal::selected(line) : Terminal::normal(line)) + "\n";
            }
            out += "\n" + Terminal::muted("[Enter] Use  [Esc] Back");
        }
    } else {
        out += tui::dim("--- Actions ---") + "\n";
        const char* acts[] = {"1. Attack","2. Use Item","3. Run"};
        for (int i = 0; i < 3; ++i)
            out += (i == cursor_ ? Terminal::selected(acts[i]) : Terminal::normal(acts[i])) + "\n";
        out += "Choice: ";
    }
    return out;
}
