/**
 * @file element.cpp
 * @brief Element system implementation
 */

#include "element.hpp"
#include "../config.hpp"
#include <config.h>

/* Element interaction lookup table [attacker][defender] */
static float g_interaction_table[static_cast<int>(Element::COUNT)]
                                [static_cast<int>(Element::COUNT)];
static bool g_initialized = false;

static void ensure_initialized() {
  if (g_initialized) {
    return;
  }

  /* Default everything to neutral */
  for (int i = 0; i < static_cast<int>(Element::COUNT); i++) {
    for (int j = 0; j < static_cast<int>(Element::COUNT); j++) {
      g_interaction_table[i][j] = DAMAGE_MOD_NEUTRAL;
    }
  }

  /* Fire: Strong vs Wind, Weak vs Water */
  g_interaction_table[static_cast<int>(Element::FIRE)]
                     [static_cast<int>(Element::WIND)] = DAMAGE_MOD_STRONG;
  g_interaction_table[static_cast<int>(Element::FIRE)]
                     [static_cast<int>(Element::WATER)] = DAMAGE_MOD_WEAK;

  /* Water: Strong vs Fire, Weak vs Earth */
  g_interaction_table[static_cast<int>(Element::WATER)]
                     [static_cast<int>(Element::FIRE)] = DAMAGE_MOD_STRONG;
  g_interaction_table[static_cast<int>(Element::WATER)]
                     [static_cast<int>(Element::EARTH)] = DAMAGE_MOD_WEAK;

  /* Earth: Strong vs Water, Weak vs Wind */
  g_interaction_table[static_cast<int>(Element::EARTH)]
                     [static_cast<int>(Element::WATER)] = DAMAGE_MOD_STRONG;
  g_interaction_table[static_cast<int>(Element::EARTH)]
                     [static_cast<int>(Element::WIND)] = DAMAGE_MOD_WEAK;

  /* Wind: Strong vs Earth, Weak vs Fire */
  g_interaction_table[static_cast<int>(Element::WIND)]
                     [static_cast<int>(Element::EARTH)] = DAMAGE_MOD_STRONG;
  g_interaction_table[static_cast<int>(Element::WIND)]
                     [static_cast<int>(Element::FIRE)] = DAMAGE_MOD_WEAK;

  g_initialized = true;
}

const char* element_to_string(Element e) {
  switch (e) {
    case Element::FIRE:
      return "Fire";
    case Element::WATER:
      return "Water";
    case Element::EARTH:
      return "Earth";
    case Element::WIND:
      return "Wind";
    case Element::NONE:
    default:
      return "None";
  }
}

float element_get_modifier(Element attacker, Element defender) {
  ensure_initialized();

  /* enum class tidak bisa langsung dijadiin index array, harus di-cast dulu */
  int atk_idx = static_cast<int>(attacker);
  int def_idx = static_cast<int>(defender);

  if (atk_idx < 0 || atk_idx >= static_cast<int>(Element::COUNT) ||
      def_idx < 0 || def_idx >= static_cast<int>(Element::COUNT)) {
    return DAMAGE_MOD_NEUTRAL;
  }

  return g_interaction_table[atk_idx][def_idx];
}

Element element_get_counter(Element e) {
  switch (e) {
    case Element::FIRE:
      return Element::WATER;
    case Element::WATER:
      return Element::EARTH;
    case Element::EARTH:
      return Element::WIND;
    case Element::WIND:
      return Element::FIRE;
    default:
      return Element::NONE;
  }
}

Element element_get_weakness(Element e) {
  switch (e) {
    case Element::FIRE:
      return Element::WIND;
    case Element::WATER:
      return Element::FIRE;
    case Element::EARTH:
      return Element::WATER;
    case Element::WIND:
      return Element::EARTH;
    default:
      return Element::NONE;
  }
}