/**
 * @file utils.hpp
 * @brief Cross-platform utility functions for TextRPG (C++ Version)
 */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

namespace Utils {

    /* ============================================
     * TIMING UTILITIES
     * ============================================ */

    /**
     * @brief Pause execution for specified milliseconds
     * @param milliseconds Duration to pause
     */
    void delay_ms(unsigned int milliseconds);

    /* ============================================
     * DISPLAY UTILITIES
     * ============================================ */

    /**
     * @brief Clear the terminal screen
     */
    void clear_screen();

    /**
     * @brief Print a horizontal separator line
     * @param width Number of characters
     * @param ch Character to use (default: '=')
     */
    void print_separator(int width, char ch = '=');

    /**
     * @brief Print centered text
     * @param text Text to center
     * @param width Total width to center within
     */
    void print_centered(const std::string& text, int width);

    /**
     * @brief Print a box header
     * @param title Title text
     * @param width Box width
     */
    void print_header(const std::string& title, int width);

    /* ============================================
     * INPUT UTILITIES
     * ============================================ */

    /**
     * @brief Get integer input from user with validation
     * @return Valid integer or -1 on invalid input
     */
    int get_int_input(int min, int max);

    /**
     * @brief Clear input buffer
     */
    void clear_input_buffer();

    /**
     * @brief Wait for user to press Enter
     */
    void wait_for_enter();

    /**
     * @brief Get yes/no confirmation
     * @return true for yes, false for no
     */
    bool get_yes_no();

    /* ============================================
     * RANDOM UTILITIES
     * ============================================ */

    int random_range(int min, int max);
    float random_float();
    bool random_chance(float chance);

    /* ============================================
     * STRING UTILITIES
     * ============================================ */

    /**
     * @brief Safe string copy (Hanya dipertahankan untuk kompatibilitas buffer C)
     */
    void safe_strcpy(char* dest, const char* src, std::size_t max_len);

} // namespace Utils

#endif // UTILS_HPP