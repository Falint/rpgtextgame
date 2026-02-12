/**
 * @file input.c
 * @brief Input handling implementation
 *
 * Implements non-blocking keyboard input:
 * - Uses select() for timeout-based polling
 * - Parses ANSI escape sequences for arrow keys
 * - Handles special keys (Ctrl+C, etc.)
 *
 * DEPENDENCIES: POSIX (select, unistd.h)
 */

#define _POSIX_C_SOURCE 200809L

#include "input.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

/* ============================================
 * CONSTANTS
 * ============================================ */

/* ASCII control characters */
#define CTRL_C 3
#define CTRL_Q 17
#define TAB_CHAR 9
#define ENTER_CHAR 13
#define ESCAPE_CHAR 27
#define BACKSPACE 127
#define SPACE_CHAR 32

/* Maximum escape sequence length */
#define MAX_ESC_SEQ 8

/* ============================================
 * INTERNAL HELPERS
 * ============================================ */

/**
 * @brief Parse escape sequence into key type
 */
static KeyType parse_escape_sequence(const char *seq, int len) {
  if (len < 2) {
    return KEY_ESCAPE; /* Just ESC key */
  }

  /* CSI sequences: ESC [ ... */
  if (seq[1] == '[') {
    if (len >= 3) {
      switch (seq[2]) {
      case 'A':
        return KEY_UP;
      case 'B':
        return KEY_DOWN;
      case 'C':
        return KEY_RIGHT;
      case 'D':
        return KEY_LEFT;
      }
    }
  }

  /* SS3 sequences: ESC O ... (some terminals) */
  if (seq[1] == 'O') {
    if (len >= 3) {
      switch (seq[2]) {
      case 'A':
        return KEY_UP;
      case 'B':
        return KEY_DOWN;
      case 'C':
        return KEY_RIGHT;
      case 'D':
        return KEY_LEFT;
      }
    }
  }

  return KEY_UNKNOWN;
}

/**
 * @brief Check if more data is available to read (non-blocking)
 */
static int data_available(int fd) {
  fd_set fds;
  struct timeval tv = {0, 0}; /* Immediate return */

  FD_ZERO(&fds);
  FD_SET(fd, &fds);

  return select(fd + 1, &fds, NULL, NULL, &tv) > 0;
}

/* ============================================
 * PUBLIC FUNCTIONS
 * ============================================ */

int input_poll(TermContext *ctx, KeyEvent *event, int timeout_ms) {
  if (!ctx || !event) {
    return -1;
  }

  /* Initialize event */
  event->type = KEY_NONE;
  event->ch = 0;

  /* Set up select with timeout */
  fd_set fds;
  struct timeval tv;

  FD_ZERO(&fds);
  FD_SET(ctx->fd, &fds);

  tv.tv_sec = timeout_ms / 1000;
  tv.tv_usec = (timeout_ms % 1000) * 1000;

  int result = select(ctx->fd + 1, &fds, NULL, NULL, &tv);

  if (result == -1) {
    if (errno == EINTR) {
      return 0; /* Interrupted, treat as timeout */
    }
    return -1; /* Error */
  }

  if (result == 0) {
    return 0; /* Timeout */
  }

  /* Data available, read it */
  return input_read_key(ctx, event);
}

int input_read_key(TermContext *ctx, KeyEvent *event) {
  if (!ctx || !event) {
    return -1;
  }

  event->type = KEY_NONE;
  event->ch = 0;

  char c;
  ssize_t n = read(ctx->fd, &c, 1);

  if (n <= 0) {
    return n == 0 ? 0 : -1;
  }

  /* Handle control characters */
  switch ((unsigned char)c) {
  case CTRL_C:
    event->type = KEY_CTRL_C;
    return 1;
  case CTRL_Q:
    event->type = KEY_CTRL_Q;
    return 1;
  case TAB_CHAR:
    event->type = KEY_TAB;
    return 1;
  case ENTER_CHAR:
  case '\n':
    event->type = KEY_ENTER;
    return 1;
  case BACKSPACE:
  case '\b':
    event->type = KEY_BACKSPACE;
    return 1;
  case SPACE_CHAR:
    event->type = KEY_SPACE;
    return 1;
  }

  /* Handle escape sequences */
  if (c == ESCAPE_CHAR) {
    char seq[MAX_ESC_SEQ];
    int seq_len = 0;
    seq[seq_len++] = c;

    /* Read additional bytes if available */
    while (seq_len < MAX_ESC_SEQ - 1 && data_available(ctx->fd)) {
      if (read(ctx->fd, &c, 1) == 1) {
        seq[seq_len++] = c;
      } else {
        break;
      }
    }
    seq[seq_len] = '\0';

    event->type = parse_escape_sequence(seq, seq_len);
    return 1;
  }

  /* Regular printable character */
  if (c >= 32 && c < 127) {
    event->type = KEY_CHAR;
    event->ch = c;
    return 1;
  }

  /* Unknown character */
  event->type = KEY_UNKNOWN;
  return 1;
}

const char *input_key_name(KeyType type) {
  switch (type) {
  case KEY_NONE:
    return "NONE";
  case KEY_UP:
    return "UP";
  case KEY_DOWN:
    return "DOWN";
  case KEY_LEFT:
    return "LEFT";
  case KEY_RIGHT:
    return "RIGHT";
  case KEY_ENTER:
    return "ENTER";
  case KEY_ESCAPE:
    return "ESCAPE";
  case KEY_BACKSPACE:
    return "BACKSPACE";
  case KEY_TAB:
    return "TAB";
  case KEY_SPACE:
    return "SPACE";
  case KEY_CHAR:
    return "CHAR";
  case KEY_CTRL_C:
    return "CTRL_C";
  case KEY_CTRL_Q:
    return "CTRL_Q";
  case KEY_UNKNOWN:
    return "UNKNOWN";
  default:
    return "???";
  }
}
