/**
 * @file enemy.hpp
 * @brief Enemy system with monster templates (C++ Version)
 */

#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "config.hpp"
#include "types.hpp"
#include "../data/registry.hpp"
#include <string_view>

/**
 * @brief Runtime enemy instance
 */
struct Enemy {
    /* ============================================
     * DATA MEMBERS
     * ============================================ */
    char id[GameConfig::Limits::MAX_ID_LEN];
    char name[GameConfig::Limits::MAX_NAME_LEN];
    int32_t max_hp;
    int32_t current_hp;
    int32_t attack;
    int32_t defense;
    Element element;
    MonsterType type;
    int32_t gold_min;
    int32_t gold_max;
    const MonsterTemplate* tmpl;

    /* ============================================
     * CONSTRUCTORS
     * ============================================ */
    
    // Default constructor untuk inisialisasi aman
    Enemy() : max_hp(0), current_hp(0), attack(0), defense(0), 
              element(Element::NONE), type(MonsterType::NORMAL), 
              gold_min(0), gold_max(0), tmpl(nullptr) {}

    /* ============================================
     * MEMBER FUNCTIONS (Methods)
     * ============================================ */

    /**
     * @brief Menerima damage
     * @return true jika musuh mati
     */
    bool take_damage(int32_t amount);

    /**
     * @brief Menghitung damage serangan musuh
     */
    int32_t calculate_attack() const;

    /**
     * @brief Mengacak jumlah gold drop
     */
    int32_t roll_gold_drop() const;

    /**
     * @brief Cek apakah musuh masih hidup
     */
    bool is_alive() const { return current_hp > 0; }

    /**
     * @brief Menampilkan status musuh ke konsol
     */
    void print_status() const;
    void print_detailed() const;
};

/* ============================================
 * FACTORY FUNCTIONS
 * Menggunakan namespace agar rapi
 * ============================================ */
namespace EnemyFactory {
    
    /**
     * @brief Membuat instance Enemy berdasarkan ID template
     */
    Enemy create(const char* template_id);

    /**
     * @brief Membuat instance Enemy dari pointer template
     */
    Enemy create_from_template(const MonsterTemplate* tmpl);

    /**
     * @brief Membuat musuh acak berdasarkan tipe tertentu
     */
    Enemy create_random(MonsterType type);

} // namespace EnemyFactory

/**
 * @brief Fungsi helper untuk konversi tipe monster ke string
 */
std::string_view monster_type_to_string(MonsterType type);

#endif // ENEMY_HPP