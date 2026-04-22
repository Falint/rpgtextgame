/**
 * @file battle.hpp
 * @brief Battle system interface (C++ Version)
 */

#ifndef BATTLE_HPP
#define BATTLE_HPP

#include "../core/enemy.hpp"
#include "../core/player.hpp"
#include "../types.hpp"

/**
 * @brief Hasil dari sebuah pertempuran.
 * Menggunakan enum class agar pemanggilan lebih jelas, 
 * contoh: BattleResult::WIN
 */
enum class BattleResult {
    WIN,
    LOSE,
    ESCAPE
};

namespace Battle {

    /**
     * @brief Memulai pertarungan antara pemain dan musuh.
     * * @param player Pointer ke objek pemain
     * @param enemy Pointer ke objek musuh
     * @return BattleResult (WIN, LOSE, atau ESCAPE)
     */
    BattleResult start(Player* player, Enemy* enemy);

    /**
     * @brief Memulai pertarungan acak berdasarkan tipe monster.
     * * @param player Pointer ke objek pemain
     * @param type Tipe monster yang akan dilawan
     * @return BattleResult
     */
    BattleResult random_encounter(Player* player, MonsterType type);

} // namespace Battle

#endif // BATTLE_HPP