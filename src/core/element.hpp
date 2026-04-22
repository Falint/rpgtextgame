/**
 * @file element.hpp
 * @brief Element system for damage calculations (C++ Version)
 */

#ifndef ELEMENT_HPP
#define ELEMENT_HPP

#include "../types.hpp"
#include <string_view>

/**
 * Namespace ElementSystem digunakan untuk mengelompokkan logika elemen
 * agar tidak berbenturan dengan enum Element yang ada di types.hpp.
 */
namespace ElementSystem {

    /**
     * @brief Mendapatkan representasi string dari elemen.
     * Menggunakan std::string_view untuk performa tinggi tanpa alokasi memori tambahan.
     * * @param e Tipe elemen
     * @return std::string_view Nama elemen (contoh: "Fire", "Water")
     */
    const std::string_view to_string(Element e);

    /**
     * @brief Menghitung pengganda damage berdasarkan elemen penyerang vs bertahan.
     * * @param attacker Elemen dari penyerang/senjata
     * @param defender Elemen dari target/musuh
     * @return float 2.0f (Kuat), 0.5f (Lemah), atau 1.0f (Netral)
     */
    float get_modifier(Element attacker, Element defender);

    /**
     * @brief Mendapatkan elemen yang kuat melawan elemen yang diberikan.
     * Contoh: Jika input adalah Water, maka mengembalikan Fire.
     */
    Element get_counter(Element e);

    /**
     * @brief Mendapatkan elemen yang lemah melawan elemen yang diberikan.
     * Contoh: Jika input adalah Fire, maka mengembalikan Water.
     */
    Element get_weakness(Element e);

} // namespace ElementSystem

#endif // ELEMENT_HPP