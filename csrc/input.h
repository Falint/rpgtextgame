/**
 * @file input.h
 * @brief Input handling API for keyboard events
 *
 * Provides non-blocking keyboard input handling:
 * - Key event types (arrows, enter, escape, etc.)
 * - Polling with timeout
 * - Escape sequence parsing
 *
 * DESIGN: All input state managed by caller (no globals)
 */

#ifndef INPUT_H
#define INPUT_H

#include "terminal.h"

/* ============================================
 * KEY TYPES
 * ============================================ */
typedef enum {
  KEY_NONE = 0, /* No key pressed (timeout) */
  KEY_UP,
  KEY_DOWN,
  KEY_LEFT,
  KEY_RIGHT,
  KEY_ENTER,
  KEY_ESCAPE,
  KEY_BACKSPACE,
  KEY_TAB,
  KEY_SPACE,
  KEY_CHAR, /* Regular character (value in ch field) */
  KEY_CTRL_C,
  KEY_CTRL_Q,
  KEY_UNKNOWN /* Unrecognized escape sequence */
} KeyType;

/* ============================================
 * KEY EVENT
 * ============================================ */
typedef struct {
  KeyType type; /* Type of key event */
  char ch;      /* Character value (for KEY_CHAR) */
} KeyEvent;

/* ============================================
 * INPUT FUNCTIONS
 * ============================================ */

/**
 * @brief Poll for keyboard input with timeout
 * @param ctx Terminal context (must be in raw mode)
 * @param event Output: key event if available
 * @param timeout_ms Maximum time to wait in milliseconds (0 = no wait)
 * @return 1 if event available, 0 if timeout, -1 on error
 */
int input_poll(TermContext *ctx, KeyEvent *event, int timeout_ms);

/**
 * @brief Read a single key (blocking)
 * @param ctx Terminal context (must be in raw mode)
 * @param event Output: key event
 * @return 1 on success, -1 on error
 */
int input_read_key(TermContext *ctx, KeyEvent *event);

/**
 * @brief Get string name of key type (for debugging)
 */
const char *input_key_name(KeyType type);

#endif /* INPUT_H */
