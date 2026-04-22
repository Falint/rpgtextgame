/**
 * @file element.cpp
 * @brief Element system implementation (C++ Version)
 */

#include "src/core/element.hpp"
#include "../config.hpp" // Menarik konstanta dari GameConfig
#include <string_view>

namespace ElementSystem {

    std::string_view to_string(Element e) {
        switch (e) {
            case ELEMENT_FIRE:   return "Fire";
            case ELEMENT_WATER:  return "Water";
            case ELEMENT_EARTH:  return "Earth";
            case ELEMENT_WIND:   return "Wind";
            case ELEMENT_LIGHT:  return "Light";
            case ELEMENT_DARK:   return "Dark";
            case ELEMENT_NONE:   
            default:             return "None";
        }
    }

    float get_modifier(Element attacker, Element defender) {
        if (attacker == ELEMENT_NONE || defender == ELEMENT_NONE) {
            return GameConfig::Combat::DAMAGE_MOD_NEUTRAL;
        }

        // Jika penyerang kuat melawan elemen bertahan
        if (get_counter(defender) == attacker) {
            return GameConfig::Combat::DAMAGE_MOD_STRONG;
        }

        // Jika penyerang lemah melawan elemen bertahan
        if (get_counter(attacker) == defender) {
            return GameConfig::Combat::DAMAGE_MOD_WEAK;
        }

        return GameConfig::Combat::DAMAGE_MOD_NEUTRAL;
    }

    Element get_counter(Element e) {
        switch (e) {
            case ELEMENT_FIRE:  return ELEMENT_WATER; // Water > Fire
            case ELEMENT_WATER: return ELEMENT_EARTH; // Earth > Water
            case ELEMENT_EARTH: return ELEMENT_WIND;  // Wind > Earth
            case ELEMENT_WIND:  return ELEMENT_FIRE;  // Fire > Wind
            case ELEMENT_LIGHT: return ELEMENT_DARK;  // Dark > Light
            case ELEMENT_DARK:  return ELEMENT_LIGHT; // Light > Dark
            default:            return ELEMENT_NONE;
        }
    }

    Element get_weakness(Element e) {
        // Logika kebalikan dari counter
        switch (e) {
            case ELEMENT_FIRE:  return ELEMENT_WIND;
            case ELEMENT_WATER: return ELEMENT_FIRE;
            case ELEMENT_EARTH: return ELEMENT_WATER;
            case ELEMENT_WIND:  return ELEMENT_EARTH;
            case ELEMENT_LIGHT: return ELEMENT_DARK;
            case ELEMENT_DARK:  return ELEMENT_LIGHT;
            default:            return ELEMENT_NONE;
        }
    }

} // namespace ElementSystem