/**
 * @file weapon.c
 * @brief Weapon system implementation
 */

#include "weapon.h"
#include "../utils/utils.h"
#include "element.h"
#include <stdio.h>
#include <string.h>

Weapon weapon_create(const char *template_id) {
  Weapon w = {0};

  const WeaponTemplate *tmpl = registry_get_weapon(template_id);
  if (!tmpl) {
    /* Return empty/default weapon */
    safe_strcpy(w.id, "fists", MAX_ID_LEN);
    safe_strcpy(w.name, "Fists", MAX_NAME_LEN);
    safe_strcpy(w.description, "Your bare hands.", MAX_DESC_LEN);
    w.type = WEAPON_TYPE_FIST;
    w.element = ELEMENT_NONE;
    w.base_damage = 5;
    w.bonus_damage = 0;
    w.tier = 1;
    w.max_tier = 1;
    w.sell_price = 0;
    return w;
  }

  return weapon_create_from_template(tmpl);
}

Weapon weapon_create_from_template(const WeaponTemplate *tmpl) {
  Weapon w = {0};

  if (!tmpl) {
    return weapon_create("fists");
  }

  safe_strcpy(w.id, tmpl->id, MAX_ID_LEN);
  safe_strcpy(w.name, tmpl->name, MAX_NAME_LEN);
  safe_strcpy(w.description, tmpl->description, MAX_DESC_LEN);
  w.type = tmpl->type;
  w.element = tmpl->element;
  w.base_damage = tmpl->base_damage;
  w.bonus_damage = 0;
  w.tier = 1;
  w.max_tier = tmpl->max_tier;
  w.sell_price = tmpl->sell_price;

  return w;
}

int weapon_get_damage(const Weapon *w) {
  if (!w)
    return 0;
  return w->base_damage + w->bonus_damage;
}

int weapon_can_upgrade(const Weapon *w) {
  if (!w)
    return 0;
  return w->tier < w->max_tier;
}

void weapon_apply_upgrade(Weapon *w, int damage_bonus) {
  if (!w || !weapon_can_upgrade(w)) {
    return;
  }

  w->tier++;
  w->bonus_damage += damage_bonus;

  /* Update sell price based on tier */
  w->sell_price = (int)(w->sell_price * 1.2f);
}

const char *weapon_type_to_string(WeaponType type) {
  switch (type) {
  case WEAPON_TYPE_FIST:
    return "Fist";
  case WEAPON_TYPE_SWORD:
    return "Sword";
  case WEAPON_TYPE_AXE:
    return "Axe";
  case WEAPON_TYPE_STAFF:
    return "Staff";
  case WEAPON_TYPE_BOW:
    return "Bow";
  case WEAPON_TYPE_DAGGER:
    return "Dagger";
  default:
    return "Unknown";
  }
}

void weapon_print(const Weapon *w) {
  if (!w)
    return;

  printf("[%s", w->name);
  if (w->tier > 1) {
    printf(" +%d", w->tier - 1);
  }
  printf("] DMG:%d", weapon_get_damage(w));

  if (w->element != ELEMENT_NONE) {
    printf(" (%s)", element_to_string(w->element));
  }
}

void weapon_print_detailed(const Weapon *w) {
  if (!w)
    return;

  printf("=== %s", w->name);
  if (w->tier > 1) {
    printf(" +%d", w->tier - 1);
  }
  printf(" ===\n");

  printf("Type: %s\n", weapon_type_to_string(w->type));
  printf("Damage: %d", weapon_get_damage(w));
  if (w->bonus_damage > 0) {
    printf(" (%d + %d)", w->base_damage, w->bonus_damage);
  }
  printf("\n");

  printf("Element: %s\n", element_to_string(w->element));
  printf("Tier: %d/%d\n", w->tier, w->max_tier);
  printf("Sell Price: %d G\n", w->sell_price);
  printf("%s\n", w->description);
}
