#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../domain/player.hpp"
#include "../domain/enemy.hpp"
#include "../data/monsters.hpp"
#include "item_service.hpp"

enum class BattleState {
    None = 0, PlayerTurn, EnemyTurn, Won, Lost, Escaped
};

static constexpr double CRIT_CHANCE     = 0.10;
static constexpr double CRIT_MULTIPLIER = 1.5;
static constexpr double ESCAPE_CHANCE   = 0.50;
static constexpr int    BATTLE_LOG_CAP  = 20;

class BattleService {
public:
    BattleService(Player* player,
                  MonsterRegistry* monsterReg,
                  ItemService*     itemService);

    void startBattle(MonsterType type);
    void attack();
    void useItem(int slotIndex);
    void tryEscape();
    void endBattle();

    BattleState           state()      const { return state_; }
    bool                  isBattleOver() const {
        return state_ == BattleState::Won ||
               state_ == BattleState::Lost ||
               state_ == BattleState::Escaped;
    }
    const Enemy*          getEnemy()   const { return enemy_.get(); }
    const std::vector<std::string>& getLogs() const { return logs_; }
    int                   turnCount()  const { return turnCount_; }
    int                   goldEarned() const { return goldEarned_; }

private:
    void enemyTurn();
    void onEnemyDefeated();
    void addLog(const std::string& msg);

    Player*          player_;
    MonsterRegistry* monsterReg_;
    ItemService*     itemService_;

    std::shared_ptr<Enemy> enemy_;
    BattleState            state_      = BattleState::None;
    std::vector<std::string> logs_;
    int                    turnCount_  = 0;
    int                    goldEarned_ = 0;
};
