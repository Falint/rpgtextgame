#pragma once
#include <string>
#include <vector>
#include <memory>
#include "inventory.hpp"
#include "errors.hpp"

// ─── Buff types ──────────────────────────────────────────────────────────────
enum class BuffType { None = 0, Attack, Defense };

struct Buff {
    BuffType type           = BuffType::None;
    int      value          = 0;  // % modifier
    int      turnsRemaining = 0;
};

// ─── Combat constants ────────────────────────────────────────────────────────
static constexpr double DEFENSE_REDUCTION_RATIO = 0.5;
static constexpr int    DEFAULT_BASE_ATTACK     = 10;
static constexpr int    MIN_DAMAGE              = 1;

struct PlayerConfig {
    int startingHP   = 100;
    int startingGold = 300;
    int baseDef      = 10;
    int maxBuffs     = 3;
};

inline PlayerConfig defaultPlayerConfig() {
    return PlayerConfig{100, 300, 10, 3};
}

// ─── Player ──────────────────────────────────────────────────────────────────
class Player {
public:
    explicit Player(const std::string& name, const PlayerConfig& cfg = defaultPlayerConfig());

    std::string              name;
    int                      maxHP     = 0;
    int                      currentHP = 0;
    int                      baseAttack = DEFAULT_BASE_ATTACK;
    int                      baseDef   = 0;
    int                      gold      = 0;
    std::shared_ptr<Weapon>  equippedWeapon = nullptr;
    std::unique_ptr<Inventory> inventory;
    std::vector<Buff>        buffs;

    bool isAlive()    const { return currentHP > 0; }
    int  getAttack()  const;
    int  getDefense() const;

    // Returns true if player died.
    bool takeDamage(int amount);
    void heal(int amount);
    void addGold(int amount)   { gold += amount; }
    ResultCode spendGold(int amount);
    void equipWeapon(std::shared_ptr<Weapon> w) { equippedWeapon = w; }
    void addBuff(BuffType type, int value, int duration);
    void tickBuffs();
};
