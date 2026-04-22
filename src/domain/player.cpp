#include "player.hpp"
#include <algorithm>

Player::Player(const std::string& n, const PlayerConfig& cfg)
    : name(n)
    , maxHP(cfg.startingHP)
    , currentHP(cfg.startingHP)
    , baseAttack(DEFAULT_BASE_ATTACK)
    , baseDef(cfg.baseDef)
    , gold(cfg.startingGold)
    , inventory(std::make_unique<Inventory>(MAX_INVENTORY_SLOTS))
{
    buffs.reserve(cfg.maxBuffs);
}

int Player::getAttack() const {
    int atk = baseAttack;
    if (equippedWeapon) atk += equippedWeapon->damage();
    for (auto& b : buffs)
        if (b.type == BuffType::Attack)
            atk += atk * b.value / 100;
    return atk;
}

int Player::getDefense() const {
    int def = baseDef;
    for (auto& b : buffs)
        if (b.type == BuffType::Defense)
            def += def * b.value / 100;
    return def;
}

bool Player::takeDamage(int amount) {
    int reduced = amount - static_cast<int>(getDefense() * DEFENSE_REDUCTION_RATIO);
    if (reduced < MIN_DAMAGE) reduced = MIN_DAMAGE;
    currentHP -= reduced;
    if (currentHP < 0) currentHP = 0;
    return currentHP <= 0;
}

void Player::heal(int amount) {
    currentHP += amount;
    if (currentHP > maxHP) currentHP = maxHP;
}

ResultCode Player::spendGold(int amount) {
    if (gold < amount) return ResultCode::NotEnoughGold;
    gold -= amount;
    return ResultCode::Success;
}

void Player::addBuff(BuffType type, int value, int duration) {
    buffs.push_back({type, value, duration});
}

void Player::tickBuffs() {
    std::vector<Buff> active;
    for (auto& b : buffs) {
        b.turnsRemaining--;
        if (b.turnsRemaining > 0) active.push_back(b);
    }
    buffs = std::move(active);
}
