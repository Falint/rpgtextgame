/**
 * @file terminal.h
 * @brief Terminal control API for raw mode and screen operations
 *
 * Provides cross-platform (POSIX) terminal control:
 * - Raw mode enable/disable
 * - Screen clearing and cursor control
 * - Terminal size detection
 * - Signal handling for resize
 *
 * DESIGN PRINCIPLES:
 * - No global state: all state in TermContext
 * - Explicit lifecycle: init/destroy pattern
 * - Clear error codes: no magic numbers
 */

#ifndef TERMINAL_H
#define TERMINAL_H

#include <termios.h>

/* ============================================
 * ERROR CODES
 * ============================================ */
typedef enum {
    TERM_OK = 0,
    TERM_ERR_NOT_TTY = -1,
    TERM_ERR_TCGETATTR = -2,
    TERM_ERR_TCSETATTR = -3,
    TERM_ERR_IOCTL = -4
} TermResult;

/* ============================================
 * TERMINAL CONTEXT
 * All terminal state is encapsulated here
 * ============================================ */
typedef struct {
    int fd;                     /* File descriptor (usually STDIN_FILENO) */
    struct termios original;    /* Original terminal settings */
    int raw_mode_active;        /* 1 if raw mode is enabled */
    int width;                  /* Terminal width in columns */
    int height;                 /* Terminal height in rows */
} TermContext;

/* ============================================
 * LIFECYCLE
 * ============================================ */

/**
 * @brief Initialize terminal context
 * @param ctx Pointer to uninitialized context
 * @return TERM_OK on success, error code on failure
 */
TermResult term_init(TermContext *ctx);

/**
 * @brief Cleanup terminal context and restore settings
 * @param ctx Initialized context
 */
void term_destroy(TermContext *ctx);

/* ============================================
 * MODE CONTROL
 * ============================================ */

/**
 * @brief Enable raw mode (no echo, no line buffering)
 * @param ctx Initialized context
 * @return TERM_OK on success, error code on failure
 */
TermResult term_enable_raw_mode(TermContext *ctx);

/**
 * @brief Disable raw mode (restore original settings)
 * @param ctx Initialized context
 * @return TERM_OK on success, error code on failure
 */
TermResult term_disable_raw_mode(TermContext *ctx);

/* ============================================
 * SCREEN OPERATIONS
 * ============================================ */

/**
 * @brief Clear the entire screen
 */
void term_clear(TermContext *ctx);

/**
 * @brief Get current terminal size
 * Updates ctx->width and ctx->height
 */
void term_get_size(TermContext *ctx);

/**
 * @brief Move cursor to specified position (1-indexed)
 */
void term_move_cursor(TermContext *ctx, int row, int col);

/**
 * @brief Hide the cursor
 */
void term_hide_cursor(TermContext *ctx);

/**
 * @brief Show the cursor
 */
void term_show_cursor(TermContext *ctx);

/**
 * @brief Move cursor to home position (top-left)
 */
void term_cursor_home(TermContext *ctx);

/**
 * @brief Flush output buffer
 */
void term_flush(TermContext *ctx);

#endif /* TERMINAL_H */
