#pragma once
#include "../domain/player.hpp"
#include "terminal.hpp"
#include <string>
#include <vector>

// ─── Layout constants (mirrors Go tui/constants.go) ──────────────────────────
inline constexpr int MENU_RATIO = 2;
inline constexpr int CHAR_RATIO = 5;
inline constexpr int GAME_RATIO = 5;
inline constexpr int HEADER_HEIGHT = 1;
inline constexpr int FOOTER_HEIGHT = 1;

// ─── Renderer: builds and prints 3-panel layout ──────────────────────────────
class Renderer {
public:
  // Call once on startup / resize
  void setSize(int cols, int rows);

  // Draws the complete frame:
  //   header bar | [ menu panel | char panel | game panel ] | footer bar
  void draw(const std::string &menuContent, const std::string &charContent,
            const std::string &gameContent, bool gameIsFocused);

  int menuWidth() const { return menuW_; }
  int charWidth() const { return charW_; }
  int gameWidth() const { return gameW_; }
  int panelHeight() const { return panelH_; }

private:
  // Build a single panel as a block of lines (no ANSI border artifacts).
  // Fills to exactly `height` lines of `width` chars.
  std::vector<std::string> buildPanel(const std::string &content, int width,
                                      int height, bool focused) const;
  std::string repeat(const std::string &s, int n) const;
  std::string padRight(const std::string &s, int w) const;
  std::string stripAnsi(const std::string &s) const;
  int visibleLen(const std::string &s) const;

  int cols_ = 120;
  int rows_ = 30;
  int menuW_ = 20;
  int charW_ = 40;
  int gameW_ = 40;
  int panelH_ = 24;
};

// ─── Character status panel renderer ─────────────────────────────────────────
std::string renderCharacterPanel(const Player *player, int width);

// ─── Menu panel renderer
// ──────────────────────────────────────────────────────
struct MenuItem {
  std::string id, label, key;
};
std::string renderMenuPanel(const std::vector<MenuItem> &items, int cursor,
                            bool focused, int width);
