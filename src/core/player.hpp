/**
 * @file player.hpp
 * @brief Player system with buffs and equipment (C++ Version)
 */

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>
#include <vector>
#include <array>

#include "../types.hpp"
#include "../config.hpp"
#include "inventory.hpp"
#include "weapon.hpp"

/**
 * @brief Active buff on player
 */
struct Buff {
    ConsumableType type;
    int32_t value;           /* Kekuatan efek (persentase/flat) */
    int32_t turns_remaining;

    Buff() : type(ConsumableType::NONE), value(0), turns_remaining(0) {}
};

/**
 * @brief Player state
 */
struct Player {
    /* ============================================
     * DATA MEMBERS
     * ============================================ */
    std::string name;
    int32_t max_hp;
    int32_t current_hp;
    int32_t base_attack;
    int32_t base_defense;
    int32_t gold;

    Inventory inventory;
    Weapon equipped_weapon;
    bool has_weapon;

    /* Active buffs menggunakan std::array agar fixed-size sesuai config */
    std::array<Buff, GameConfig::Buffs::MAX_ACTIVE> active_buffs;
    int32_t buff_count;

    /* ============================================
     * CONSTRUCTOR
     * ============================================ */
    
    /**
     * @brief Constructor untuk inisialisasi awal pemain
     */
    Player(const std::string& player_name = "Hero");

    /* ============================================
     * MEMBER FUNCTIONS (Methods)
     * ============================================ */

    /**
     * @brief Kalkulasi statistik total (Base + Weapon + Buffs)
     */
    int32_t get_total_attack() const;
    int32_t get_total_defense() const;

    /**
     * @brief Sistem Equipment & Items
     */
    ResultCode equip_weapon(int32_t inventory_slot);
    ResultCode use_item(int32_t inventory_slot);

    /**
     * @brief Combat & Health
     */
    bool take_damage(int32_t amount);
    void heal(int32_t amount);
    void full_restore();
    bool is_alive() const { return current_hp > 0; }

    /**
     * @brief Ekonomi
     */
    void add_gold(int32_t amount);
    ResultCode spend_gold(int32_t amount);

    /**
     * @brief Sistem Buff
     */
    void add_buff(ConsumableType type, int32_t value, int32_t duration);
    void tick_buffs();
    bool has_buff(ConsumableType type) const;

    /**
     * @brief UI / Display
     */
    void print_status() const;
    void print_detailed() const;
    void print_buffs() const;
};

#endif // PLAYER_HPP