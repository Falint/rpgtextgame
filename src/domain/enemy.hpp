#pragma once
#include <string>
#include <vector>
#include "element.hpp"

enum class MonsterType { Normal = 0, Elite, Boss };

inline const char* monsterTypeStr(MonsterType t) {
    switch (t) {
    case MonsterType::Elite: return "Elite";
    case MonsterType::Boss:  return "BOSS";
    default:                 return "Normal";
    }
}

struct Enemy {
    std::string id;
    std::string name;
    MonsterType type      = MonsterType::Normal;
    Element     element   = Element::None;
    int         maxHP     = 0;
    int         currentHP = 0;
    int         attack    = 0;
    int         defense   = 0;
    int         goldMin   = 0;
    int         goldMax   = 0;

    bool isAlive()      const { return currentHP > 0; }
    int  getAttack()    const { return attack; }
    Element getElement()const { return element; }

    // Returns true if enemy died.
    bool takeDamage(int amount);

    // Random gold in [goldMin, goldMax].
    int rollGold() const;
};
