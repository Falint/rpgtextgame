#include <ctime>
#include <memory>

#include "domain/player.hpp"
#include "data/monsters.hpp"
#include "data/weapons.hpp"
#include "data/items.hpp"
#include "data/upgrades.hpp"
#include "game/item_service.hpp"
#include "game/battle_service.hpp"
#include "game/shop_service.hpp"
#include "tui/app.hpp"

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // ── Registries ────────────────────────────────────────────────────────────
    MonsterRegistry monsters;
    WeaponRegistry  weapons;
    ItemRegistry    items;
    UpgradeRegistry upgrades;

    // ── Player ────────────────────────────────────────────────────────────────
    auto player = std::make_shared<Player>("Hero");

    // Starting gear — give the player a rusty sword + a couple potions
    auto* startWep = weapons.getByID("rusty_sword");
    if (startWep) player->equippedWeapon = startWep->toWeapon();

    auto* sp = items.getByID("small_potion");
    if (sp) player->inventory->addItem(sp->toItem(), 3);

    // ── Services ──────────────────────────────────────────────────────────────
    ItemService   itemSvc;
    BattleService battle(player.get(), &monsters, &itemSvc, &items);
    ShopService   shop(player.get(), &weapons, &items);

    // ── TUI App ───────────────────────────────────────────────────────────────
    App app(player, &monsters, &weapons, &items, &upgrades,
            &battle, &shop, &itemSvc);
    app.run();

    return 0;
}