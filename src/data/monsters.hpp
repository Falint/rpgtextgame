#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "../domain/enemy.hpp"
#include "../domain/element.hpp"

struct LootEntry {
    std::string itemID;
    float       dropChance = 0.f;
};

struct MonsterTemplate {
    std::string          id;
    std::string          name;
    MonsterType          type    = MonsterType::Normal;
    Element              element = Element::None;
    int                  baseHP  = 0;
    int                  baseAtk = 0;
    int                  baseDef = 0;
    int                  goldMin = 0;
    int                  goldMax = 0;
    std::vector<LootEntry> lootTable;

    // Create a mutable enemy instance from this template.
    std::shared_ptr<Enemy> toEnemy() const {
        auto e = std::make_shared<Enemy>();
        e->id        = id;
        e->name      = name;
        e->type      = type;
        e->element   = element;
        e->maxHP     = baseHP;
        e->currentHP = baseHP;
        e->attack    = baseAtk;
        e->defense   = baseDef;
        e->goldMin   = goldMin;
        e->goldMax   = goldMax;
        return e;
    }
};

class MonsterRegistry {
public:
    MonsterRegistry();

    const MonsterTemplate* getByID(const std::string& id) const;
    std::vector<const MonsterTemplate*> getByType(MonsterType t) const;
    // Returns a random template of the given type (nullptr if none).
    const MonsterTemplate* getRandom(MonsterType t) const;

private:
    void load();
    std::map<std::string, MonsterTemplate>              monsters_;
    std::map<MonsterType, std::vector<MonsterTemplate*>> byType_;
};