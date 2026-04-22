/**
 * @file weapon.h
 * @brief Weapon system with upgrade support
 */

#ifndef WEAPON_H
#define WEAPON_H

#include "../data/registry.h"
#include "../types.h"

/**
 * @brief Runtime weapon instance
 */
struct Weapon {
  char id[MAX_ID_LEN];     /* Template ID for lookups */
  char name[MAX_NAME_LEN]; /* Display name (may include tier) */
  char description[MAX_DESC_LEN];
  WeaponType type;
  Element element;
  int base_damage;  /* Original base damage */
  int bonus_damage; /* Damage from upgrades */
  int tier;         /* Current upgrade tier (1-5) */
  int max_tier;
  int sell_price;
};

/**
 * @brief Create weapon from template ID
 */
Weapon weapon_create(const char *template_id);

/**
 * @brief Create weapon directly from template
 */
Weapon weapon_create_from_template(const WeaponTemplate *tmpl);

/**
 * @brief Get total current damage (base + bonus)
 */
int weapon_get_damage(const Weapon *w);

/**
 * @brief Check if weapon can be upgraded
 */
int weapon_can_upgrade(const Weapon *w);

/**
 * @brief Apply upgrade to weapon (increments tier and adds bonus)
 */
void weapon_apply_upgrade(Weapon *w, int damage_bonus);

/**
 * @brief Get weapon type as string
 */
const char *weapon_type_to_string(WeaponType type);

/**
 * @brief Print weapon info (single line)
 */
void weapon_print(const Weapon *w);

/**
 * @brief Print detailed weapon info (multi-line)
 */
void weapon_print_detailed(const Weapon *w);

#endif /* WEAPON_H */
