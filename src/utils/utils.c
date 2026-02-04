/**
 * @file utils.c
 * @brief Cross-platform utility functions implementation
 */

/* Feature test macro for nanosleep on POSIX systems */
#ifndef _WIN32
#define _POSIX_C_SOURCE 199309L
#endif

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

/* ============================================
 * TIMING UTILITIES
 * ============================================ */

void delay_ms(unsigned int milliseconds) {
#ifdef _WIN32
  Sleep(milliseconds);
#else
  struct timespec ts;
  ts.tv_sec = milliseconds / 1000;
  ts.tv_nsec = (milliseconds % 1000) * 1000000L;
  nanosleep(&ts, NULL);
#endif
}

/* ============================================
 * DISPLAY UTILITIES
 * ============================================ */

void clear_screen(void) {
  /*
   * ANSI escape codes:
   * \033[2J - Clear entire screen
   * \033[H  - Move cursor to home position (top-left)
   */
  printf("\033[2J\033[H");
  fflush(stdout);
}

void print_separator(int width, char ch) {
  for (int i = 0; i < width; i++) {
    putchar(ch);
  }
  putchar('\n');
}

void print_centered(const char *text, int width) {
  int len = (int)strlen(text);
  int padding = (width - len) / 2;

  if (padding < 0) {
    padding = 0;
  }

  for (int i = 0; i < padding; i++) {
    putchar(' ');
  }
  printf("%s\n", text);
}

void print_header(const char *title, int width) {
  print_separator(width, '=');
  print_centered(title, width);
  print_separator(width, '=');
}

/* ============================================
 * INPUT UTILITIES
 * ============================================ */

void clear_input_buffer(void) {
  int c;
  while ((c = getchar()) != '\n' && c != EOF) {
    /* Discard */
  }
}

int get_int_input(int min, int max) {
  int value;
  if (scanf("%d", &value) != 1) {
    clear_input_buffer();
    return -1;
  }

  if (value < min || value > max) {
    return -1;
  }

  return value;
}

void wait_for_enter(void) {
  printf("\nPress Enter to continue...");
  clear_input_buffer();
  getchar();
}

int get_yes_no(void) {
  printf(" (y/n): ");
  char c;
  if (scanf(" %c", &c) != 1) {
    clear_input_buffer();
    return 0;
  }
  return (c == 'y' || c == 'Y');
}

/* ============================================
 * RANDOM UTILITIES
 * ============================================ */

int random_range(int min, int max) {
  if (min >= max) {
    return min;
  }
  return min + (rand() % (max - min + 1));
}

float random_float(void) { return (float)rand() / (float)RAND_MAX; }

int random_chance(float chance) { return random_float() < chance; }

/* ============================================
 * STRING UTILITIES
 * ============================================ */

void safe_strcpy(char *dest, const char *src, int max_len) {
  if (!dest || !src || max_len <= 0) {
    return;
  }
  strncpy(dest, src, max_len - 1);
  dest[max_len - 1] = '\0';
}
