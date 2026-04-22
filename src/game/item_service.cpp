#include "item_service.hpp"
#include <string>

UseResult ItemService::useConsumable(Player* player, const Item* item) {
    if (!item || item->category != ItemCategory::Consumable)
        return {false, "", "This item cannot be used."};

    switch (item->consumableType) {
    case ConsumableType::Heal:        return applyHeal(player, item);
    case ConsumableType::FullRestore: return applyFullRestore(player);
    case ConsumableType::BuffAtk:     return applyBuff(player, BuffType::Attack,  item);
    case ConsumableType::BuffDef:     return applyBuff(player, BuffType::Defense, item);
    default:
        return {false, "", "This item cannot be used."};
    }
}

UseResult ItemService::applyHeal(Player* player, const Item* item) {
    if (player->currentHP >= player->maxHP)
        return {false, "", "HP is already full!"};
    int oldHP = player->currentHP;
    player->heal(item->value);
    int healed = player->currentHP - oldHP;
    return {true,
        "Used " + item->name + "! Healed " + std::to_string(healed) +
        " HP. (HP: " + std::to_string(player->currentHP) + "/" +
        std::to_string(player->maxHP) + ")", ""};
}

UseResult ItemService::applyFullRestore(Player* player) {
    if (player->currentHP >= player->maxHP)
        return {false, "", "HP is already full!"};
    int healed = player->maxHP - player->currentHP;
    player->currentHP = player->maxHP;
    return {true,
        "Full Restore! Healed " + std::to_string(healed) +
        " HP. (HP: " + std::to_string(player->maxHP) + "/" +
        std::to_string(player->maxHP) + ")", ""};
}

UseResult ItemService::applyBuff(Player* player, BuffType buffType, const Item* item) {
    player->addBuff(buffType, item->value, item->duration);
    std::string typeName = (buffType == BuffType::Attack) ? "ATK" : "DEF";
    return {true,
        "Used " + item->name + "! " + typeName +
        " +" + std::to_string(item->value) + "% for " +
        std::to_string(item->duration) + " turns.", ""};
}
