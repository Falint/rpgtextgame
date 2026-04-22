#pragma once
#include <string>

// ─── ANSI color/style codes ──────────────────────────────────────────────────
#define ANSI_RESET       "\033[0m"
#define ANSI_BOLD        "\033[1m"
#define ANSI_DIM         "\033[2m"
#define ANSI_ITALIC      "\033[3m"
#define ANSI_STRIKE      "\033[9m"

// Foreground colors (256-color)
#define ANSI_FG(n)       "\033[38;5;" #n "m"
#define ANSI_BG(n)       "\033[48;5;" #n "m"

// Named colors matching Go lipgloss palette
#define CLR_PRIMARY      "\033[38;5;62m"   // soft blue
#define CLR_SECONDARY    "\033[38;5;241m"  // gray
#define CLR_ACCENT       "\033[38;5;205m"  // pink
#define CLR_GOLD         "\033[38;5;220m"  // gold
#define CLR_HP           "\033[38;5;196m"  // red
#define CLR_SUCCESS      "\033[38;5;42m"   // green
#define CLR_WARNING      "\033[38;5;214m"  // orange
#define CLR_DANGER       "\033[38;5;196m"  // red
#define CLR_MUTED        "\033[38;5;240m"  // dark gray
#define CLR_FIRE         "\033[38;5;202m"  // orange-red
#define CLR_WATER        "\033[38;5;39m"   // cyan
#define CLR_EARTH        "\033[38;5;136m"  // brown
#define CLR_WIND         "\033[38;5;48m"   // light green
#define CLR_TEXT         "\033[38;5;252m"  // light gray
#define CLR_DIM          "\033[38;5;245m"  // dimmed
#define CLR_WHITE        "\033[38;5;255m"  // white
#define CLR_SEL_BG       "\033[48;5;62m"   // selection bg
#define CLR_BAR_BG       "\033[48;5;236m"  // status bar bg

// ─── Cursor/screen control ───────────────────────────────────────────────────
#define ANSI_CLEAR       "\033[2J\033[H"
#define ANSI_CLEAR_SCREEN "\033[2J"
#define ANSI_HOME        "\033[H"
#define ANSI_CURSOR_POS(r,c) "\033[" #r ";" #c "H"
#define ANSI_HIDE_CURSOR "\033[?25l"
#define ANSI_SHOW_CURSOR "\033[?25h"
#define ANSI_ALT_SCREEN  "\033[?1049h"
#define ANSI_NORM_SCREEN "\033[?1049l"

// ─── Box-drawing characters ──────────────────────────────────────────────────
#define BOX_TL  "\xe2\x95\xad"   // ╭
#define BOX_TR  "\xe2\x95\xae"   // ╮
#define BOX_BL  "\xe2\x95\xb0"   // ╰
#define BOX_BR  "\xe2\x95\xaf"   // ╯
#define BOX_H   "\xe2\x94\x80"   // ─
#define BOX_V   "\xe2\x94\x82"   // │

// Key codes returned by readKey()
enum class Key {
    Unknown = 0,
    Up, Down, Left, Right,
    Enter, Escape, Backspace,
    K1,K2,K3,K4,K5,K6,K7,K8,K9,
    CtrlC, CtrlQ,
    Char,   // printable character, use lastChar()
};

// ─── Terminal namespace ───────────────────────────────────────────────────────
namespace Terminal {
    void enableRawMode();
    void disableRawMode();
    void enableANSI();     // Windows: enable VT processing
    void getSize(int& cols, int& rows);
    Key  readKey(char& outChar);  // blocking read; fills outChar for Key::Char
    void flush();                 // flush stdout

    // Style helpers — return styled string
    inline std::string bold(const std::string& s)    { return std::string(ANSI_BOLD)    + s + ANSI_RESET; }
    inline std::string dim(const std::string& s)     { return std::string(ANSI_DIM)     + s + ANSI_RESET; }
    inline std::string color(const char* c, const std::string& s) { return std::string(c) + s + ANSI_RESET; }
    inline std::string selected(const std::string& s) {
        return std::string(CLR_SEL_BG) + ANSI_BOLD + CLR_WHITE + " " + s + " " + ANSI_RESET;
    }
    inline std::string normal(const std::string& s)  { return std::string(CLR_TEXT) + " " + s + " " + ANSI_RESET; }
    inline std::string success(const std::string& s) { return std::string(CLR_SUCCESS) + ANSI_BOLD + s + ANSI_RESET; }
    inline std::string error(const std::string& s)   { return std::string(CLR_DANGER)  + ANSI_BOLD + s + ANSI_RESET; }
    inline std::string gold(const std::string& s)    { return std::string(CLR_GOLD)    + ANSI_BOLD + s + ANSI_RESET; }
    inline std::string accent(const std::string& s)  { return std::string(CLR_ACCENT)  + ANSI_BOLD + s + ANSI_RESET; }
    inline std::string muted(const std::string& s)   { return std::string(CLR_MUTED)   + s + ANSI_RESET; }
    inline std::string hpColor(int cur, int max, const std::string& s) {
        double pct = max > 0 ? static_cast<double>(cur)/max : 0.0;
        if (pct <= 0.3) return std::string(CLR_DANGER) + ANSI_BOLD + s + ANSI_RESET;
        return std::string(CLR_SUCCESS) + s + ANSI_RESET;
    }
} // namespace Terminal
