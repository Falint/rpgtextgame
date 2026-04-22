#include "enemy.hpp"
#include <cstdlib>
#include <algorithm>

bool Enemy::takeDamage(int amount) {
    int reduced = amount - static_cast<int>(defense * 0.5);
    if (reduced < 1) reduced = 1;
    currentHP -= reduced;
    if (currentHP < 0) currentHP = 0;
    return currentHP <= 0;
}

int Enemy::rollGold() const {
    if (goldMax <= goldMin) return goldMin;
    return goldMin + (std::rand() % (goldMax - goldMin + 1));
}
