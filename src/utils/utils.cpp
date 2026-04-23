/**
 * @file utils.hpp
 * @brief Cross-platform utility functions implementation (C++ Version)
 */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <string>
#include <string_view>
#include <thread>
#include <chrono>
#include <limits>
#include <random>

namespace Utils {

    /* ============================================
     * TIMING UTILITIES
     * ============================================ */

    /**
     * Menggunakan std::this_thread agar lebih cross-platform 
     * tanpa perlu #ifdef manual untuk Windows/Linux.
     */
    inline void delay_ms(unsigned int milliseconds) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    /* ============================================
     * DISPLAY UTILITIES
     * ============================================ */

    inline void clear_screen() {
        // Tetap menggunakan ANSI escape codes karena efisien untuk terminal modern
        std::cout << "\033[2J\033[H" << std::flush;
    }

    inline void print_separator(int width, char ch = '=') {
        std::cout << std::string(width, ch) << "\n";
    }

    inline void print_centered(const std::string& text, int width) {
        int len = static_cast<int>(text.length());
        int padding = (width - len) / 2;

        if (padding > 0) {
            std::cout << std::string(padding, ' ');
        }
        std::cout << text << "\n";
    }

    inline void print_header(const std::string& title, int width) {
        print_separator(width, '=');
        print_centered(title, width);
        print_separator(width, '=');
    }

    /* ============================================
     * INPUT UTILITIES
     * ============================================ */

    inline void clear_input_buffer() {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    inline int get_int_input(int min, int max) {
        int value;
        if (!(std::cin >> value)) {
            std::cin.clear(); // Reset error flag
            clear_input_buffer();
            return -1;
        }

        if (value < min || value > max) {
            return -1;
        }

        return value;
    }

    inline void wait_for_enter() {
        std::cout << "\nPress Enter to continue...";
        std::cin.clear();
        clear_input_buffer();
        std::cin.get();
    }

    inline bool get_yes_no() {
        std::cout << " (y/n): ";
        char c;
        if (!(std::cin >> c)) {
            std::cin.clear();
            clear_input_buffer();
            return false;
        }
        return (c == 'y' || c == 'Y');
    }

    /* ============================================
     * RANDOM UTILITIES
     * Menggunakan <random> (Mersenne Twister) untuk hasil yang lebih baik dari rand()
     * ============================================ */

    inline int random_range(int min, int max) {
        if (min >= max) return min;
        static std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<> dist(min, max);
        return dist(gen);
    }

    inline float random_float() {
        static std::mt19937 gen(std::random_device{}());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        return dist(gen);
    }

    inline bool random_chance(float chance) {
        return random_float() < chance;
    }

    /* ============================================
     * STRING UTILITIES
     * Dalam C++, kita sebaiknya menggunakan std::string, 
     * fungsi ini dipertahankan hanya jika kamu masih menggunakan array char.
     * ============================================ */

    inline void safe_strcpy(char* dest, const char* src, std::size_t max_len) {
        if (!dest || !src || max_len == 0) return;
        const std::string s(src);
        std::size_t copy_len = std::min(s.length(), max_len - 1);
        s.copy(dest, copy_len);
        dest[copy_len] = '\0';
    }

} // namespace Utils

#endif // UTILS_HPP