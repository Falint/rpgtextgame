/**
 * @file terminal.c
 * @brief Terminal control implementation using POSIX termios
 *
 * Implements raw terminal mode for TUI applications:
 * - Disables canonical mode (line buffering)
 * - Disables echo
 * - Sets minimum read bytes and timeout
 * - Handles terminal resize via ioctl
 *
 * DEPENDENCIES: POSIX (termios.h, sys/ioctl.h, unistd.h)
 */

#define _POSIX_C_SOURCE 200809L

#include "terminal.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

/* ============================================
 * ANSI ESCAPE SEQUENCES
 * ============================================ */
#define ESC "\033"
#define CSI ESC "["

/* Screen control */
#define ANSI_CLEAR_SCREEN CSI "2J"
#define ANSI_CURSOR_HOME CSI "H"
#define ANSI_HIDE_CURSOR CSI "?25l"
#define ANSI_SHOW_CURSOR CSI "?25h"

/* Default terminal dimensions as fallback */
#define DEFAULT_TERM_WIDTH 80
#define DEFAULT_TERM_HEIGHT 24

/* ============================================
 * LIFECYCLE
 * ============================================ */

TermResult term_init(TermContext *ctx) {
  if (!ctx) {
    return TERM_ERR_NOT_TTY;
  }

  /* Initialize struct to known state */
  memset(ctx, 0, sizeof(TermContext));
  ctx->fd = STDIN_FILENO;
  ctx->raw_mode_active = 0;
  ctx->width = DEFAULT_TERM_WIDTH;
  ctx->height = DEFAULT_TERM_HEIGHT;

  /* Check if stdin is a terminal */
  if (!isatty(ctx->fd)) {
    return TERM_ERR_NOT_TTY;
  }

  /* Save original terminal settings */
  if (tcgetattr(ctx->fd, &ctx->original) == -1) {
    return TERM_ERR_TCGETATTR;
  }

  /* Get initial terminal size */
  term_get_size(ctx);

  return TERM_OK;
}

void term_destroy(TermContext *ctx) {
  if (!ctx) {
    return;
  }

  /* Restore original settings if we modified them */
  if (ctx->raw_mode_active) {
    term_disable_raw_mode(ctx);
  }

  /* Show cursor in case it was hidden */
  term_show_cursor(ctx);

  /* Flush any pending output */
  term_flush(ctx);
}

/* ============================================
 * MODE CONTROL
 * ============================================ */

TermResult term_enable_raw_mode(TermContext *ctx) {
  if (!ctx || ctx->raw_mode_active) {
    return TERM_OK; /* Already in raw mode */
  }

  struct termios raw = ctx->original;

  /*
   * Input modes:
   * - IXON: Disable Ctrl-S/Ctrl-Q flow control
   * - ICRNL: Disable CR to NL translation
   * - BRKINT: Disable break condition signals
   * - INPCK: Disable parity checking
   * - ISTRIP: Disable stripping 8th bit
   */
  raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);

  /*
   * Output modes:
   * - OPOST: Disable output processing
   */
  raw.c_oflag &= ~(OPOST);

  /*
   * Control modes:
   * - CS8: 8 bits per byte
   */
  raw.c_cflag |= (CS8);

  /*
   * Local modes:
   * - ECHO: Disable echo
   * - ICANON: Disable canonical mode (line buffering)
   * - ISIG: Disable signal generation (Ctrl-C, Ctrl-Z)
   * - IEXTEN: Disable Ctrl-V
   */
  raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

  /*
   * Control characters:
   * - VMIN: Minimum bytes to read (0 = non-blocking)
   * - VTIME: Timeout in deciseconds (1 = 100ms)
   */
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if (tcsetattr(ctx->fd, TCSAFLUSH, &raw) == -1) {
    return TERM_ERR_TCSETATTR;
  }

  ctx->raw_mode_active = 1;
  return TERM_OK;
}

TermResult term_disable_raw_mode(TermContext *ctx) {
  if (!ctx || !ctx->raw_mode_active) {
    return TERM_OK; /* Already in normal mode */
  }

  if (tcsetattr(ctx->fd, TCSAFLUSH, &ctx->original) == -1) {
    return TERM_ERR_TCSETATTR;
  }

  ctx->raw_mode_active = 0;
  return TERM_OK;
}

/* ============================================
 * SCREEN OPERATIONS
 * ============================================ */

void term_clear(TermContext *ctx) {
  (void)ctx; /* Unused but kept for consistency */
  write(STDOUT_FILENO, ANSI_CLEAR_SCREEN, strlen(ANSI_CLEAR_SCREEN));
  write(STDOUT_FILENO, ANSI_CURSOR_HOME, strlen(ANSI_CURSOR_HOME));
}

void term_get_size(TermContext *ctx) {
  if (!ctx) {
    return;
  }

  struct winsize ws;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
    ctx->width = ws.ws_col;
    ctx->height = ws.ws_row;
  } else {
    /* Fallback to defaults */
    ctx->width = DEFAULT_TERM_WIDTH;
    ctx->height = DEFAULT_TERM_HEIGHT;
  }

  /* Sanity check */
  if (ctx->width < 20)
    ctx->width = 20;
  if (ctx->height < 10)
    ctx->height = 10;
}

void term_move_cursor(TermContext *ctx, int row, int col) {
  (void)ctx;
  char buf[32];
  int len = snprintf(buf, sizeof(buf), CSI "%d;%dH", row, col);
  if (len > 0 && len < (int)sizeof(buf)) {
    write(STDOUT_FILENO, buf, len);
  }
}

void term_hide_cursor(TermContext *ctx) {
  (void)ctx;
  write(STDOUT_FILENO, ANSI_HIDE_CURSOR, strlen(ANSI_HIDE_CURSOR));
}

void term_show_cursor(TermContext *ctx) {
  (void)ctx;
  write(STDOUT_FILENO, ANSI_SHOW_CURSOR, strlen(ANSI_SHOW_CURSOR));
}

void term_cursor_home(TermContext *ctx) {
  (void)ctx;
  write(STDOUT_FILENO, ANSI_CURSOR_HOME, strlen(ANSI_CURSOR_HOME));
}

void term_flush(TermContext *ctx) {
  (void)ctx;
  fflush(stdout);
}
