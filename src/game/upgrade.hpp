/**
 * @file upgrade.hpp
 * @brief Weapon upgrade system interface (C++ Version)
 */

#ifndef UPGRADE_HPP
#define UPGRADE_HPP

#include "../core/player.hpp"
#include "../core/weapon.hpp"
#include "../types.hpp"

namespace Upgrade {

    /**
     * @brief Masuk ke menu upgrade senjata.
     * @param player Pointer ke objek pemain.
     * @return MenuResult::BACK saat pemain keluar dari menu.
     */
    MenuResult enter_upgrade_menu(Player* player);

    /**
     * @brief Memeriksa apakah pemain memenuhi syarat untuk upgrade senjata.
     * @param player Pointer ke objek pemain (const).
     * @param weapon_slot Indeks slot di inventory, atau -1 untuk senjata yang sedang dipakai.
     * @return true jika emas dan material cukup, false jika tidak.
     */
    bool can_upgrade_weapon(const Player* player, int weapon_slot);

    /**
     * @brief Melakukan proses upgrade senjata (mengurangi emas/material dan menambah damage).
     * @param player Pointer ke objek pemain.
     * @param weapon_slot Indeks slot di inventory, atau -1 untuk senjata yang sedang dipakai.
     * @return ResultCode (SUCCESS, NOT_ENOUGH_GOLD, dll).
     */
    ResultCode perform_upgrade(Player* player, int weapon_slot);

    /**
     * @brief Mencetak rincian kebutuhan upgrade ke konsol.
     * @param weapon Pointer ke senjata yang ingin diperiksa (const).
     * @param player Pointer ke pemain untuk membandingkan jumlah material yang dimiliki (const).
     */
    void print_requirements(const Weapon* weapon, const Player* player);

} // namespace Upgrade

#endif // UPGRADE_HPP