/**
 * @file renderer.h
 * @brief Rendering primitives API for TUI
 *
 * Provides low-level rendering operations:
 * - Text output with colors
 * - Box drawing with Unicode borders
 * - Horizontal and vertical lines
 * - Screen buffer management
 *
 * DESIGN: Stateless functions operating on TermContext
 */

#ifndef RENDERER_H
#define RENDERER_H

#include "terminal.h"

/* ============================================
 * COLOR CODES (ANSI 16-color palette)
 * ============================================ */
typedef enum {
  COLOR_DEFAULT = -1,
  COLOR_BLACK = 0,
  COLOR_RED,
  COLOR_GREEN,
  COLOR_YELLOW,
  COLOR_BLUE,
  COLOR_MAGENTA,
  COLOR_CYAN,
  COLOR_WHITE,
  /* Bright variants */
  COLOR_BRIGHT_BLACK = 8,
  COLOR_BRIGHT_RED,
  COLOR_BRIGHT_GREEN,
  COLOR_BRIGHT_YELLOW,
  COLOR_BRIGHT_BLUE,
  COLOR_BRIGHT_MAGENTA,
  COLOR_BRIGHT_CYAN,
  COLOR_BRIGHT_WHITE
} Color;

/* Text attributes */
typedef enum {
  ATTR_NONE = 0,
  ATTR_BOLD = 1,
  ATTR_DIM = 2,
  ATTR_UNDERLINE = 4,
  ATTR_REVERSE = 8
} TextAttr;

/* ============================================
 * RECTANGLE
 * ============================================ */
typedef struct {
  int x; /* Column (1-indexed) */
  int y; /* Row (1-indexed) */
  int width;
  int height;
} Rect;

/* ============================================
 * RENDERING FUNCTIONS
 * ============================================ */

/**
 * @brief Reset all text attributes to default
 */
void render_reset(TermContext *ctx);

/**
 * @brief Set foreground and background colors
 */
void render_set_color(TermContext *ctx, Color fg, Color bg);

/**
 * @brief Set text attributes (bold, underline, etc.)
 */
void render_set_attr(TermContext *ctx, TextAttr attr);

/**
 * @brief Output text at position
 */
void render_text(TermContext *ctx, int row, int col, const char *text);

/**
 * @brief Output text with color
 */
void render_text_color(TermContext *ctx, int row, int col, const char *text,
                       Color fg, Color bg);

/**
 * @brief Draw a box with Unicode borders
 */
void render_box(TermContext *ctx, Rect rect, const char *title);

/**
 * @brief Draw horizontal line
 */
void render_hline(TermContext *ctx, int row, int col, int width, char ch);

/**
 * @brief Draw vertical line
 */
void render_vline(TermContext *ctx, int row, int col, int height, char ch);

/**
 * @brief Fill rectangle with character
 */
void render_fill(TermContext *ctx, Rect rect, char ch);

/**
 * @brief Clear rectangle (fill with spaces)
 */
void render_clear_rect(TermContext *ctx, Rect rect);

/**
 * @brief Calculate layout widths based on percentages
 * @param total_width Total available width
 * @param percentages Array of percentages (must sum to 1.0)
 * @param count Number of panels
 * @param out_widths Output array of calculated widths
 */
void render_calc_widths(int total_width, const float *percentages, int count,
                        int *out_widths);

#endif /* RENDERER_H */
