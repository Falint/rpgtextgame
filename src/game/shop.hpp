/**
 * @file shop.hpp
 * @brief Shop system interface (C++ Version)
 */

#ifndef SHOP_HPP
#define SHOP_HPP

#include "../core/player.hpp"
#include "../types.hpp"


namespace Shop {

    /**
     * @brief Masuk ke menu toko.
     * * Fungsi ini mengelola loop internal toko di mana pemain bisa 
     * membeli senjata, membeli item, atau menjual barang.
     * * @param player Pointer ke objek Player yang melakukan transaksi.
     * @return MenuResult Mengembalikan MenuResult::BACK saat keluar dari toko.
     */
    MenuResult enter(Player* player);

} // namespace Shop

#endif // SHOP_HPP