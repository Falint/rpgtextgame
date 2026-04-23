#pragma once
#include <string>
#include <memory>
#include "../domain/player.hpp"
#include "../domain/inventory.hpp"

struct UseResult {
    bool        success = false;
    std::string message;
    std::string error;
};

// Single source of truth for consumable effects.
// Does NOT remove items — callers handle removal.
class ItemService {
public:
    UseResult useConsumable(Player* player, const Item* item);

private:
    UseResult applyHeal(Player* player, const Item* item);
    UseResult applyFullRestore(Player* player);
    UseResult applyBuff(Player* player, BuffType buffType, const Item* item);
};
