#include "battle_service.hpp"
#include <cstdlib>
#include <string>
#include <sstream>

BattleService::BattleService(Player* player,
                             MonsterRegistry* monsterReg,
                             ItemService* itemService)
    : player_(player), monsterReg_(monsterReg), itemService_(itemService)
{
    logs_.reserve(BATTLE_LOG_CAP);
}

void BattleService::startBattle(MonsterType type) {
    auto* tmpl = monsterReg_->getRandom(type);
    if (!tmpl) return;
    enemy_      = tmpl->toEnemy();
    state_      = BattleState::PlayerTurn;
    logs_.clear();
    turnCount_  = 0;
    goldEarned_ = 0;
    addLog("A wild " + enemy_->name + " appears!");
    if (enemy_->element != Element::None)
        addLog(std::string("Element: ") + elementStr(enemy_->element));
}

void BattleService::attack() {
    if (state_ != BattleState::PlayerTurn) return;

    int baseDmg = player_->getAttack();

    double elemMod = 1.0;
    if (player_->equippedWeapon)
        elemMod = damageModifier(player_->equippedWeapon->element, enemy_->element);
    int dmg = static_cast<int>(baseDmg * elemMod);

    bool crit = (static_cast<double>(std::rand()) / RAND_MAX) < CRIT_CHANCE;
    if (crit) dmg = static_cast<int>(dmg * CRIT_MULTIPLIER);

    bool killed = enemy_->takeDamage(dmg);

    std::string msg = "You attack " + enemy_->name +
                      " for " + std::to_string(dmg) + " damage!";
    if (crit)          msg = "CRITICAL HIT! " + msg;
    if (elemMod > 1.0) msg += " (Super effective!)";
    if (elemMod < 1.0) msg += " (Not very effective...)";
    addLog(msg);

    if (killed) onEnemyDefeated();
    else        enemyTurn();
}

void BattleService::useItem(int slotIndex) {
    if (state_ != BattleState::PlayerTurn) return;

    auto* slot = player_->inventory->getSlot(slotIndex);
    if (!slot || slot->type != SlotType::ItemSlot) {
        addLog("Invalid item slot!"); return;
    }

    auto result = itemService_->useConsumable(player_, slot->item.get());
    if (result.success) {
        addLog(result.message);
        player_->inventory->removeItem(slotIndex, 1);
        enemyTurn();
    } else {
        addLog(result.error);
    }
}

void BattleService::tryEscape() {
    if (state_ != BattleState::PlayerTurn) return;
    if (enemy_->type == MonsterType::Boss) {
        addLog("Cannot escape from a Boss battle!"); return;
    }
    if ((static_cast<double>(std::rand()) / RAND_MAX) < ESCAPE_CHANCE) {
        state_ = BattleState::Escaped;
        addLog("You escaped successfully!");
    } else {
        addLog("Failed to escape!");
        enemyTurn();
    }
}

void BattleService::endBattle() {
    state_ = BattleState::None;
    player_->tickBuffs();
}

void BattleService::enemyTurn() {
    state_ = BattleState::EnemyTurn;
    int dmg = enemy_->getAttack();
    bool died = player_->takeDamage(dmg);
    addLog(enemy_->name + " attacks you for " + std::to_string(dmg) + " damage!");
    if (died) {
        state_ = BattleState::Lost;
        addLog("You have been defeated...");
        return;
    }
    ++turnCount_;
    player_->tickBuffs();
    state_ = BattleState::PlayerTurn;
}

void BattleService::onEnemyDefeated() {
    state_ = BattleState::Won;
    int gold = enemy_->rollGold();
    goldEarned_ = gold;
    player_->addGold(gold);
    addLog(enemy_->name + " has been defeated!");
    addLog("You earned " + std::to_string(gold) + " gold!");
}

void BattleService::addLog(const std::string& msg) {
    logs_.push_back(msg);
    if (static_cast<int>(logs_.size()) > BATTLE_LOG_CAP)
        logs_.erase(logs_.begin());
}
