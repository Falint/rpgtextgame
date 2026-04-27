#pragma once
#include "../data/monsters.hpp"
#include "../domain/enemy.hpp"
#include "../domain/player.hpp"
#include "../data/items.hpp"
#include "item_service.hpp"
#include <memory>
#include <string>
#include <vector>

enum class BattleState { None = 0, PlayerTurn, EnemyTurn, Won, Lost, Escaped };

class BattleService {
public:
  BattleService(Player *player, MonsterRegistry *monsterReg,
                ItemService *itemService, ItemRegistry *itemReg);

  void startBattle(MonsterType type);
  void attack();
  void useItem(int slotIndex);
  void tryEscape();
  void endBattle();

  BattleState state() const { return state_; }
  bool isBattleOver() const {
    return state_ == BattleState::Won || state_ == BattleState::Lost ||
           state_ == BattleState::Escaped;
  }
  const Enemy *getEnemy() const { return enemy_.get(); }
  const std::vector<std::string> &getLogs() const { return logs_; }
  int turnCount() const { return turnCount_; }
  int goldEarned() const { return goldEarned_; }

private:
  void enemyTurn();
  void onEnemyDefeated();
  void addLog(const std::string &msg);

  Player *player_;
  MonsterRegistry *monsterReg_;
  ItemService *itemService_;
  ItemRegistry *itemReg_;

  std::shared_ptr<Enemy> enemy_;
  BattleState state_ = BattleState::None;
  std::vector<std::string> logs_;
  int turnCount_ = 0;
  int goldEarned_ = 0;
};
