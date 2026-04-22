/**
 * @file shop_stock.hpp
 * @brief Shop inventory declarations (C++ Version)
 */

#ifndef SHOP_STOCK_HPP
#define SHOP_STOCK_HPP

#include "registry.hpp"
#include <vector>

/**
 * Namespace ShopDatabase mengelola daftar barang yang tersedia untuk dibeli.
 */
namespace ShopDatabase {

    /**
     * @brief Mendapatkan akses ke seluruh database stok toko.
     * @param out_count Output: jumlah entri dalam toko.
     * @return Pointer ke array ShopEntry statis.
     */
    const ShopEntry* get_database(int32_t* out_count);

    /**
     * @brief Mendapatkan daftar item yang tersedia sebagai vector.
     * Berguna untuk iterasi modern di C++.
     */
    std::vector<const ShopEntry*> get_all_entries();

} // namespace ShopDatabase

#endif // SHOP_STOCK_HPP