/**
 * @file element.c
 * @brief Element system implementation
 */

#include "element.h"
#include "../config.h"

/* Element interaction lookup table [attacker][defender] */
static float g_interaction_table[ELEMENT_COUNT][ELEMENT_COUNT];
static int g_initialized = 0;

static void ensure_initialized(void) {
  if (g_initialized) {
    return;
  }

  /* Default everything to neutral */
  for (int i = 0; i < ELEMENT_COUNT; i++) {
    for (int j = 0; j < ELEMENT_COUNT; j++) {
      g_interaction_table[i][j] = DAMAGE_MOD_NEUTRAL;
    }
  }

  /* Fire: Strong vs Wind, Weak vs Water */
  g_interaction_table[ELEMENT_FIRE][ELEMENT_WIND] = DAMAGE_MOD_STRONG;
  g_interaction_table[ELEMENT_FIRE][ELEMENT_WATER] = DAMAGE_MOD_WEAK;

  /* Water: Strong vs Fire, Weak vs Earth */
  g_interaction_table[ELEMENT_WATER][ELEMENT_FIRE] = DAMAGE_MOD_STRONG;
  g_interaction_table[ELEMENT_WATER][ELEMENT_EARTH] = DAMAGE_MOD_WEAK;

  /* Earth: Strong vs Water, Weak vs Wind */
  g_interaction_table[ELEMENT_EARTH][ELEMENT_WATER] = DAMAGE_MOD_STRONG;
  g_interaction_table[ELEMENT_EARTH][ELEMENT_WIND] = DAMAGE_MOD_WEAK;

  /* Wind: Strong vs Earth, Weak vs Fire */
  g_interaction_table[ELEMENT_WIND][ELEMENT_EARTH] = DAMAGE_MOD_STRONG;
  g_interaction_table[ELEMENT_WIND][ELEMENT_FIRE] = DAMAGE_MOD_WEAK;

  g_initialized = 1;
}

const char *element_to_string(Element e) {
  switch (e) {
  case ELEMENT_FIRE:
    return "Fire";
  case ELEMENT_WATER:
    return "Water";
  case ELEMENT_EARTH:
    return "Earth";
  case ELEMENT_WIND:
    return "Wind";
  case ELEMENT_NONE:
  default:
    return "None";
  }
}

float element_get_modifier(Element attacker, Element defender) {
  ensure_initialized();

  if (attacker < 0 || attacker >= ELEMENT_COUNT || defender < 0 ||
      defender >= ELEMENT_COUNT) {
    return DAMAGE_MOD_NEUTRAL;
  }

  return g_interaction_table[attacker][defender];
}

Element element_get_counter(Element e) {
  switch (e) {
  case ELEMENT_FIRE:
    return ELEMENT_WATER;
  case ELEMENT_WATER:
    return ELEMENT_EARTH;
  case ELEMENT_EARTH:
    return ELEMENT_WIND;
  case ELEMENT_WIND:
    return ELEMENT_FIRE;
  default:
    return ELEMENT_NONE;
  }
}

Element element_get_weakness(Element e) {
  switch (e) {
  case ELEMENT_FIRE:
    return ELEMENT_WIND;
  case ELEMENT_WATER:
    return ELEMENT_FIRE;
  case ELEMENT_EARTH:
    return ELEMENT_WATER;
  case ELEMENT_WIND:
    return ELEMENT_EARTH;
  default:
    return ELEMENT_NONE;
  }
}
