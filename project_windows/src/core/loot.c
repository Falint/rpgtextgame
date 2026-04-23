/**
 * @file loot.c
 * @brief Loot drop system implementation
 */

#include "loot.h"
#include "../data/registry.h"
#include "../utils/utils.h"
#include <stdio.h>

LootDrop loot_generate(const Enemy *enemy) {
  LootDrop loot = {0};

  if (!enemy || !enemy->template) {
    loot.gold = random_range(10, 20);
    return loot;
  }

  /* Roll gold */
  loot.gold = enemy_roll_gold(enemy);

  /* Roll for item drops */
  const MonsterTemplate *tmpl = enemy->template;
  loot.item_count = 0;

  for (int i = 0; i < tmpl->loot_count && loot.item_count < MAX_LOOT_ENTRIES;
       i++) {
    if (random_chance(tmpl->loot[i].drop_chance)) {
      Item dropped = item_create(tmpl->loot[i].item_id);
      if (dropped.id[0] != '\0') { /* Valid item */
        loot.items[loot.item_count++] = dropped;
      }
    }
  }

  return loot;
}

void loot_collect(Player *player, const LootDrop *loot) {
  if (!player || !loot)
    return;

  /* Add gold */
  player_add_gold(player, loot->gold);

  /* Add items to inventory */
  for (int i = 0; i < loot->item_count; i++) {
    ResultCode result =
        inventory_add_item(&player->inventory, loot->items[i], 1);
    if (result == RESULT_INVENTORY_FULL) {
      printf("Inventory full! Could not pick up %s.\n", loot->items[i].name);
    }
  }
}

void loot_print(const LootDrop *loot) {
  if (!loot)
    return;

  printf("\n+++ LOOT +++\n");
  printf("  Gold: %d\n", loot->gold);

  if (loot->item_count > 0) {
    printf("  Items:\n");
    for (int i = 0; i < loot->item_count; i++) {
      printf("    - %s\n", loot->items[i].name);
    }
  }
  printf("++++++++++++\n");
}
