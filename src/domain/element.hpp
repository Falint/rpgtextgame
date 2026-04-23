#pragma once

enum class Element { None = 0, Fire, Water, Earth, Wind };

inline const char* elementStr(Element e) {
    switch (e) {
    case Element::Fire:  return "Fire";
    case Element::Water: return "Water";
    case Element::Earth: return "Earth";
    case Element::Wind:  return "Wind";
    default:             return "None";
    }
}

// Returns damage multiplier: 2.0 = strong, 0.5 = weak, 1.0 = neutral.
// Cycle: Fire > Wind > Earth > Water > Fire
inline double damageModifier(Element attacker, Element target) {
    if (attacker == Element::None || target == Element::None) return 1.0;
    switch (attacker) {
    case Element::Fire:
        if (target == Element::Wind)  return 2.0;
        if (target == Element::Water) return 0.5;
        break;
    case Element::Water:
        if (target == Element::Fire)  return 2.0;
        if (target == Element::Earth) return 0.5;
        break;
    case Element::Earth:
        if (target == Element::Water) return 2.0;
        if (target == Element::Wind)  return 0.5;
        break;
    case Element::Wind:
        if (target == Element::Earth) return 2.0;
        if (target == Element::Fire)  return 0.5;
        break;
    default: break;
    }
    return 1.0;
}
