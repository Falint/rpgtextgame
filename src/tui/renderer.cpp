#include "renderer.hpp"
#include <cstdio>
#include <sstream>
#include <algorithm>
#include <string>

// ─── setSize ─────────────────────────────────────────────────────────────────
void Renderer::setSize(int cols, int rows) {
    cols_   = cols;
    rows_   = rows;
    int total  = cols_ - 4; // 2 chars margin each side for borders
    menuW_  = total * MENU_RATIO / 12;
    int rem = total - menuW_;
    charW_  = rem / 2;
    gameW_  = rem - charW_;
    panelH_ = rows_ - HEADER_HEIGHT - FOOTER_HEIGHT - 2; // 2 for panel borders
}

// ─── stripAnsi (count printable width) ───────────────────────────────────────
int Renderer::visibleLen(const std::string& s) const {
    int len = 0; bool inEsc = false;
    for (unsigned char c : s) {
        if (inEsc) { if (c == 'm') inEsc = false; continue; }
        if (c == '\033') { inEsc = true; continue; }
        if (c >= 0x80) { // multi-byte UTF-8 — skip continuation bytes
            if ((c & 0xC0) == 0x80) continue;  // continuation
            ++len; continue;
        }
        ++len;
    }
    return len;
}

std::string Renderer::padRight(const std::string& s, int w) const {
    int vis = visibleLen(s);
    if (vis >= w) return s;
    return s + std::string(w - vis, ' ');
}

std::string Renderer::repeat(const std::string& s, int n) const {
    std::string r; r.reserve(s.size() * n);
    for (int i = 0; i < n; ++i) r += s;
    return r;
}

// ─── buildPanel: wrap content into a bordered box ────────────────────────────
std::vector<std::string> Renderer::buildPanel(const std::string& content,
                                               int width, int height,
                                               bool focused) const {
    const char* borderColor = focused ? CLR_PRIMARY : CLR_SECONDARY;
    int inner = width - 2; // subtract 2 border chars

    // Split content into lines
    std::vector<std::string> lines;
    std::istringstream ss(content);
    std::string line;
    while (std::getline(ss, line)) lines.push_back(line);

    std::vector<std::string> result;
    // Top border
    result.push_back(std::string(borderColor) + BOX_TL +
                     repeat(BOX_H, inner) + BOX_TR + ANSI_RESET);

    // Content lines (padded/truncated to inner width)
    int contentH = height - 2;
    for (int i = 0; i < contentH; ++i) {
        std::string row = (i < static_cast<int>(lines.size())) ? lines[i] : "";
        std::string padded = padRight(row, inner);
        // Ensure we don't overshoot
        result.push_back(std::string(borderColor) + BOX_V + ANSI_RESET +
                         padded +
                         std::string(borderColor) + BOX_V + ANSI_RESET);
    }

    // Bottom border
    result.push_back(std::string(borderColor) + BOX_BL +
                     repeat(BOX_H, inner) + BOX_BR + ANSI_RESET);

    return result;
}

// ─── draw ────────────────────────────────────────────────────────────────────
void Renderer::draw(const std::string& menuContent,
                    const std::string& charContent,
                    const std::string& gameContent,
                    bool gameIsFocused) {
    // Build three panel blocks
    auto menuLines = buildPanel(menuContent, menuW_, panelH_ + 2, !gameIsFocused);
    auto charLines = buildPanel(charContent, charW_, panelH_ + 2, false);
    auto gameLines = buildPanel(gameContent, gameW_, panelH_ + 2, gameIsFocused);

    int totalLines = static_cast<int>(menuLines.size());

    // Header
    std::string header = std::string(CLR_BAR_BG) + CLR_DIM;
    header += " TEXT RPG  [Ctrl+C Quit] [Arrows/jk Navigate] [Enter Select]";
    // pad to width
    int hlen = visibleLen(header) - static_cast<int>(std::string(CLR_BAR_BG).size())
                                  - static_cast<int>(std::string(CLR_DIM).size());
    header += std::string(std::max(0, cols_ - hlen), ' ');
    header += ANSI_RESET;

    // Move cursor home, hide cursor while drawing
    std::string frame;
    frame.reserve(cols_ * rows_ * 8);
    frame += ANSI_HOME;
    frame += header + "\n";

    // Panel rows
    for (int r = 0; r < totalLines; ++r) {
        auto get = [&](const std::vector<std::string>& v) -> const std::string& {
            static const std::string empty;
            return (r < static_cast<int>(v.size())) ? v[r] : empty;
        };
        frame += get(menuLines) + get(charLines) + get(gameLines) + "\n";
    }

    // Footer
    std::string footer = std::string(CLR_BAR_BG) + CLR_DIM;
    footer += " [1-9] Quick Select  [Esc] Back  [k/j] Up/Down";
    int flen = visibleLen(footer) - static_cast<int>(std::string(CLR_BAR_BG).size())
                                  - static_cast<int>(std::string(CLR_DIM).size());
    footer += std::string(std::max(0, cols_ - flen), ' ');
    footer += ANSI_RESET;
    frame += footer;

    fputs(frame.c_str(), stdout);
    Terminal::flush();
}

// ─── renderCharacterPanel ─────────────────────────────────────────────────────
std::string renderCharacterPanel(const Player* p, int /*width*/) {
    if (!p) return "No player data\n";

    auto lbl = [](const std::string& s) {
        return std::string(CLR_DIM) + s + ANSI_RESET;
    };

    std::string out;
    out += Terminal::accent("CHARACTER STATUS") + "\n\n";

    // HP
    int cur = p->currentHP, mx = p->maxHP;
    double pct = mx > 0 ? static_cast<double>(cur)/mx : 0.0;
    std::string hpStr = std::to_string(cur) + "/" + std::to_string(mx);
    out += lbl("HP: ") + Terminal::hpColor(cur, mx, hpStr) + "\n";

    // HP bar
    int barW = 20;
    int filled = static_cast<int>(pct * barW);
    if (filled < 0) filled = 0;
    std::string bar = std::string(filled, '#') + std::string(barW - filled, '.');
    out += Terminal::hpColor(cur, mx, "[" + bar + "]") + "\n";

    // Gold
    out += lbl("Gold: ") + Terminal::gold(std::to_string(p->gold) + " G") + "\n\n";

    // ATK/DEF
    out += lbl("ATK: ") + std::to_string(p->getAttack()) + "\n";
    out += lbl("DEF: ") + std::to_string(p->getDefense()) + "\n\n";

    // Weapon
    out += lbl("Weapon: ");
    if (p->equippedWeapon) {
        auto& w = *p->equippedWeapon;
        const char* clr = CLR_TEXT;
        switch (w.element) {
        case Element::Fire:  clr = CLR_FIRE;  break;
        case Element::Water: clr = CLR_WATER; break;
        case Element::Earth: clr = CLR_EARTH; break;
        case Element::Wind:  clr = CLR_WIND;  break;
        default: break;
        }
        out += std::string(clr) + w.displayName() + ANSI_RESET;
        out += lbl(" (+") + std::to_string(w.damage()) + lbl(")");
    } else {
        out += Terminal::muted("None");
    }
    out += "\n";

    // Buffs
    if (!p->buffs.empty()) {
        out += lbl("Buffs: ");
        for (auto& b : p->buffs) {
            std::string name = (b.type == BuffType::Attack) ? "ATK UP" : "DEF UP";
            out += std::string(CLR_PRIMARY) + name + "(" +
                   std::to_string(b.turnsRemaining) + ")" + ANSI_RESET + " ";
        }
        out += "\n";
    }

    // ASCII art
    out += "\n";
    out += Terminal::muted("   O  \n  /|\\ \n  / \\ \n");

    return out;
}

// ─── renderMenuPanel ─────────────────────────────────────────────────────────
std::string renderMenuPanel(const std::vector<MenuItem>& items, int cursor,
                             bool focused, int /*width*/) {
    std::string out;
    out += Terminal::accent("=== MAIN MENU ===") + "\n";

    for (int i = 0; i < static_cast<int>(items.size()); ++i) {
        std::string line = items[i].key + ". " + items[i].label;
        if (i == cursor && focused)
            out += Terminal::selected(line);
        else
            out += Terminal::normal(line);
        out += "\n";
    }
    out += Terminal::muted("=================") + "\n";
    out += "Choice: ";
    return out;
}
