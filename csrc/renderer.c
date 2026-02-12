/**
 * @file renderer.c
 * @brief Rendering primitives implementation
 *
 * Implements TUI rendering using ANSI escape codes:
 * - Color output (16-color palette)
 * - Box drawing with Unicode characters
 * - Text positioning and attributes
 *
 * DEPENDENCIES: POSIX (write, unistd.h)
 */

#define _POSIX_C_SOURCE 200809L

#include "renderer.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* ============================================
 * ANSI ESCAPE SEQUENCES
 * ============================================ */
#define ESC "\033"
#define CSI ESC "["

/* Reset all attributes */
#define ANSI_RESET CSI "0m"

/* Box drawing characters (Unicode) */
#define BOX_TL "┌"
#define BOX_TR "┐"
#define BOX_BL "└"
#define BOX_BR "┘"
#define BOX_H "─"
#define BOX_V "│"

/* ============================================
 * INTERNAL HELPERS
 * ============================================ */

static void write_str(const char *s) { write(STDOUT_FILENO, s, strlen(s)); }

static void set_cursor(int row, int col) {
  char buf[32];
  int len = snprintf(buf, sizeof(buf), CSI "%d;%dH", row, col);
  if (len > 0) {
    write(STDOUT_FILENO, buf, len);
  }
}

/* ============================================
 * PUBLIC FUNCTIONS
 * ============================================ */

void render_reset(TermContext *ctx) {
  (void)ctx;
  write_str(ANSI_RESET);
}

void render_set_color(TermContext *ctx, Color fg, Color bg) {
  (void)ctx;
  char buf[32];
  int len = 0;

  if (fg >= 0 && fg < 8) {
    len = snprintf(buf, sizeof(buf), CSI "%dm", 30 + fg);
  } else if (fg >= 8 && fg < 16) {
    len = snprintf(buf, sizeof(buf), CSI "%dm", 90 + (fg - 8));
  }
  if (len > 0) {
    write(STDOUT_FILENO, buf, len);
  }

  if (bg >= 0 && bg < 8) {
    len = snprintf(buf, sizeof(buf), CSI "%dm", 40 + bg);
  } else if (bg >= 8 && bg < 16) {
    len = snprintf(buf, sizeof(buf), CSI "%dm", 100 + (bg - 8));
  }
  if (len > 0) {
    write(STDOUT_FILENO, buf, len);
  }
}

void render_set_attr(TermContext *ctx, TextAttr attr) {
  (void)ctx;
  char buf[32];
  int len;

  if (attr & ATTR_BOLD) {
    len = snprintf(buf, sizeof(buf), CSI "1m");
    write(STDOUT_FILENO, buf, len);
  }
  if (attr & ATTR_DIM) {
    len = snprintf(buf, sizeof(buf), CSI "2m");
    write(STDOUT_FILENO, buf, len);
  }
  if (attr & ATTR_UNDERLINE) {
    len = snprintf(buf, sizeof(buf), CSI "4m");
    write(STDOUT_FILENO, buf, len);
  }
  if (attr & ATTR_REVERSE) {
    len = snprintf(buf, sizeof(buf), CSI "7m");
    write(STDOUT_FILENO, buf, len);
  }
}

void render_text(TermContext *ctx, int row, int col, const char *text) {
  (void)ctx;
  if (!text)
    return;
  set_cursor(row, col);
  write_str(text);
}

void render_text_color(TermContext *ctx, int row, int col, const char *text,
                       Color fg, Color bg) {
  render_set_color(ctx, fg, bg);
  render_text(ctx, row, col, text);
  render_reset(ctx);
}

void render_box(TermContext *ctx, Rect rect, const char *title) {
  if (rect.width < 2 || rect.height < 2)
    return;

  /* Top border */
  set_cursor(rect.y, rect.x);
  write_str(BOX_TL);
  for (int i = 0; i < rect.width - 2; i++) {
    write_str(BOX_H);
  }
  write_str(BOX_TR);

  /* Title if provided */
  if (title && strlen(title) > 0) {
    int title_len = strlen(title);
    int title_pos = rect.x + 2;
    if (title_len < rect.width - 4) {
      set_cursor(rect.y, title_pos);
      write_str(" ");
      write_str(title);
      write_str(" ");
    }
  }

  /* Side borders */
  for (int i = 1; i < rect.height - 1; i++) {
    set_cursor(rect.y + i, rect.x);
    write_str(BOX_V);
    set_cursor(rect.y + i, rect.x + rect.width - 1);
    write_str(BOX_V);
  }

  /* Bottom border */
  set_cursor(rect.y + rect.height - 1, rect.x);
  write_str(BOX_BL);
  for (int i = 0; i < rect.width - 2; i++) {
    write_str(BOX_H);
  }
  write_str(BOX_BR);
}

void render_hline(TermContext *ctx, int row, int col, int width, char ch) {
  (void)ctx;
  set_cursor(row, col);
  for (int i = 0; i < width; i++) {
    char buf[2] = {ch, '\0'};
    write_str(buf);
  }
}

void render_vline(TermContext *ctx, int row, int col, int height, char ch) {
  (void)ctx;
  for (int i = 0; i < height; i++) {
    set_cursor(row + i, col);
    char buf[2] = {ch, '\0'};
    write_str(buf);
  }
}

void render_fill(TermContext *ctx, Rect rect, char ch) {
  (void)ctx;
  for (int y = 0; y < rect.height; y++) {
    set_cursor(rect.y + y, rect.x);
    for (int x = 0; x < rect.width; x++) {
      char buf[2] = {ch, '\0'};
      write_str(buf);
    }
  }
}

void render_clear_rect(TermContext *ctx, Rect rect) {
  render_fill(ctx, rect, ' ');
}

void render_calc_widths(int total_width, const float *percentages, int count,
                        int *out_widths) {
  if (!percentages || !out_widths || count <= 0)
    return;

  int used = 0;
  for (int i = 0; i < count - 1; i++) {
    out_widths[i] = (int)(total_width * percentages[i]);
    if (out_widths[i] < 5)
      out_widths[i] = 5; /* Minimum width */
    used += out_widths[i];
  }
  /* Last panel gets remaining space */
  out_widths[count - 1] = total_width - used;
  if (out_widths[count - 1] < 5) {
    out_widths[count - 1] = 5;
  }
}
