/**
 * @file utils.h
 * @brief Cross-platform utility functions for TextRPG
 *
 * Provides portable delay, screen clearing, and input functions
 * that work on both Windows and Linux/Unix systems.
 */

#ifndef UTILS_H
#define UTILS_H

/* ============================================
 * TIMING UTILITIES
 * ============================================ */

/**
 * @brief Pause execution for specified milliseconds
 *
 * Cross-platform delay implementation using:
 * - Sleep() on Windows
 * - nanosleep() on Linux/Unix
 *
 * @param milliseconds Duration to pause
 */
void delay_ms(unsigned int milliseconds);

/* ============================================
 * DISPLAY UTILITIES
 * ============================================ */

/**
 * @brief Clear the terminal screen
 *
 * Uses ANSI escape codes for portability.
 */
void clear_screen(void);

/**
 * @brief Print a horizontal separator line
 *
 * @param width Number of characters
 * @param ch Character to use (e.g., '=', '-')
 */
void print_separator(int width, char ch);

/**
 * @brief Print centered text
 *
 * @param text Text to center
 * @param width Total width to center within
 */
void print_centered(const char *text, int width);

/**
 * @brief Print a box header
 *
 * @param title Title text
 * @param width Box width
 */
void print_header(const char *title, int width);

/* ============================================
 * INPUT UTILITIES
 * ============================================ */

/**
 * @brief Get integer input from user with validation
 *
 * @param min Minimum valid value
 * @param max Maximum valid value
 * @return Valid integer or -1 on invalid input
 */
int get_int_input(int min, int max);

/**
 * @brief Clear input buffer
 */
void clear_input_buffer(void);

/**
 * @brief Wait for user to press Enter
 */
void wait_for_enter(void);

/**
 * @brief Get yes/no confirmation
 *
 * @return 1 for yes, 0 for no
 */
int get_yes_no(void);

/* ============================================
 * RANDOM UTILITIES
 * ============================================ */

/**
 * @brief Get random integer in range [min, max]
 */
int random_range(int min, int max);

/**
 * @brief Get random float in range [0.0, 1.0]
 */
float random_float(void);

/**
 * @brief Check if random roll succeeds
 *
 * @param chance Probability (0.0 to 1.0)
 * @return 1 if roll succeeds, 0 otherwise
 */
int random_chance(float chance);

/* ============================================
 * STRING UTILITIES
 * ============================================ */

/**
 * @brief Safe string copy with null termination
 */
void safe_strcpy(char *dest, const char *src, int max_len);

#endif /* UTILS_H */
