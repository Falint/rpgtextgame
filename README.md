# TextRPG

A professional, data-driven text-based RPG written in C99.

## Features

- **Turn-Based Combat** with elemental strengths/weaknesses
- **14 Monsters** across 3 tiers (Normal, Elite, Boss)
- **19 Weapons** with elemental affinities
- **Weapon Upgrade System** - upgrade weapons using materials dropped by monsters
- **Buff System** - ATK/DEF potions with turn-based duration
- **Shop System** - Buy weapons, potions, and sell loot
- **Loot Drops** - Each monster has unique loot tables

## Build

```bash
# Build the game
make

# Build and run
make run

# Clean build
make rebuild
```

## Architecture

The codebase follows a professional **layered architecture**:

```
src/
├── main.c           # Entry point
├── types.h          # Type definitions
├── config.h         # Game constants
├── utils/           # Utility functions
├── data/            # Data layer (Registry pattern)
├── core/            # Core entities (Player, Enemy, Item, etc.)
└── game/            # Game systems (Battle, Shop, Upgrade)
```

### Data-Driven Design

All game content is defined in static databases:
- `data/monsters.c` - Monster definitions
- `data/weapons.c` - Weapon definitions  
- `data/items.c` - Item definitions
- `data/upgrades.c` - Upgrade recipes
- `data/shop_stock.c` - Shop inventory

To add new content, simply edit the appropriate data file. No code changes required!

## How to Play

1. **Battle** - Fight monsters to earn gold and materials
2. **Shop** - Buy better weapons and potions
3. **Upgrade** - Use materials to upgrade your weapons
4. **Win** - Defeat the bosses!

### Combat Tips

- Use elemental advantages (Fire > Wind > Earth > Water > Fire)
- Elite monsters drop rare materials for upgrades
- Boss monsters cannot be escaped but give the best rewards
- Use buff potions before tough fights

## Controls

All interactions are number-based menu selections. Simply type the number of your choice and press Enter.

## Requirements

- GCC with C99 support
- Make

## License

MIT License
