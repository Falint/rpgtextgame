// Package registry provides read-only data registries for game entities.
// All data definitions EXACTLY match the C src/data/*.c files to ensure
// feature parity between the C and Go versions of the game.
//
// DESIGN PATTERN: Repository/Registry
// Each registry loads its data at initialization and provides typed query
// methods for lookup, filtering, and shop stock management.
package registry

import "github.com/tenyom/textrpg-tui/internal/domain" // Domain types for item categories, consumable types

// ItemTemplate is an immutable item definition loaded at initialization.
// EXACT MATCH: C src/data/items.h ItemTemplate struct.
// Templates are never modified at runtime — mutable domain.Item instances
// are created from templates via ToItem() for inventory placement.
type ItemTemplate struct {
	ID             string                // Unique key (e.g., "small_potion", "slime_gel")
	Name           string                // Display name for shop/inventory UI
	Description    string                // Flavor text for item details
	Category       domain.ItemCategory   // ItemConsumable or ItemMaterial
	ConsumableType domain.ConsumableType // Effect dispatcher (ConsumeHeal, ConsumeBuffAtk, etc.)
	Value          int                   // Effect magnitude: heal amount or buff percentage
	Duration       int                   // Buff duration in combat turns (0 for non-buff items)
	BuyPrice       int                   // Shop purchase cost in gold (0 = not purchasable)
	SellPrice      int                   // Gold received when selling this item
	Stackable      bool                  // Whether multiple units share one inventory slot
}

// ToItem creates a mutable domain.Item from this immutable template.
// Used by ShopService.BuyItem() and giveStartingItems() to create
// new item instances for the player's inventory.
func (t *ItemTemplate) ToItem() *domain.Item {
	return &domain.Item{
		ID:             t.ID,             // Copy unique identifier
		Name:           t.Name,           // Copy display name
		Description:    t.Description,    // Copy flavor text
		Category:       t.Category,       // Copy category (Consumable/Material)
		ConsumableType: t.ConsumableType, // Copy effect type
		Value:          t.Value,          // Copy effect magnitude
		Duration:       t.Duration,       // Copy buff duration
		BuyPrice:       t.BuyPrice,       // Copy purchase price
		SellPrice:      t.SellPrice,      // Copy sell price
		Stackable:      t.Stackable,      // Copy stacking behavior
	}
}

// ItemRegistry is the read-only data store for all item definitions.
// Loaded once at application startup and shared across all services.
type ItemRegistry struct {
	items     map[string]*ItemTemplate // All items indexed by ID for O(1) lookup
	shopStock map[string]int           // Shop availability: -1 = unlimited, 0 = sold out, >0 = remaining
}

// NewItemRegistry creates and initializes the item registry.
// Loads all item definitions and shop stock levels, matching C items.c data.
func NewItemRegistry() *ItemRegistry {
	r := &ItemRegistry{
		items:     make(map[string]*ItemTemplate), // Initialize lookup map
		shopStock: make(map[string]int),           // Initialize stock tracker
	}
	r.loadItems()     // Populate item definitions (30+ items)
	r.initShopStock() // Set initial shop stock levels
	return r
}

// loadItems initializes all item data.
// EXACT MATCH: C src/data/items.c ITEM_DB[]
func (r *ItemRegistry) loadItems() {
	items := []*ItemTemplate{
		// =========================================
		// CONSUMABLES - Healing Potions
		// =========================================
		{
			ID: "small_potion", Name: "Small Potion",
			Description:    "A basic healing potion. Restores 30 HP.",
			Category:       domain.ItemConsumable,
			ConsumableType: domain.ConsumeHeal,
			Value:          30, Duration: 0, BuyPrice: 25, SellPrice: 10, Stackable: true,
		},
		{
			ID: "medium_potion", Name: "Medium Potion",
			Description:    "A standard healing potion. Restores 60 HP.",
			Category:       domain.ItemConsumable,
			ConsumableType: domain.ConsumeHeal,
			Value:          60, Duration: 0, BuyPrice: 50, SellPrice: 20, Stackable: true,
		},
		{
			ID: "large_potion", Name: "Large Potion",
			Description:    "A powerful healing potion. Restores 100 HP.",
			Category:       domain.ItemConsumable,
			ConsumableType: domain.ConsumeHeal,
			Value:          100, Duration: 0, BuyPrice: 100, SellPrice: 40, Stackable: true,
		},
		{
			ID: "full_restore", Name: "Full Restore",
			Description:    "Completely restores HP and cures all status.",
			Category:       domain.ItemConsumable,
			ConsumableType: domain.ConsumeFullRestore,
			Value:          999, Duration: 0, BuyPrice: 300, SellPrice: 120, Stackable: true,
		},

		// =========================================
		// CONSUMABLES - Buff Potions
		// =========================================
		{
			ID: "atk_elixir", Name: "Attack Elixir",
			Description:    "Boosts ATK by 25% for 3 turns.",
			Category:       domain.ItemConsumable,
			ConsumableType: domain.ConsumeBuffAtk,
			Value:          25, Duration: 3, BuyPrice: 60, SellPrice: 25, Stackable: true,
		},
		{
			ID: "def_elixir", Name: "Defense Elixir",
			Description:    "Boosts DEF by 25% for 3 turns.",
			Category:       domain.ItemConsumable,
			ConsumableType: domain.ConsumeBuffDef,
			Value:          25, Duration: 3, BuyPrice: 60, SellPrice: 25, Stackable: true,
		},
		{
			ID: "power_surge", Name: "Power Surge",
			Description:    "Massive ATK boost of 50% for 2 turns.",
			Category:       domain.ItemConsumable,
			ConsumableType: domain.ConsumeBuffAtk,
			Value:          50, Duration: 2, BuyPrice: 120, SellPrice: 50, Stackable: true,
		},
		{
			ID: "iron_skin", Name: "Iron Skin Potion",
			Description:    "Massive DEF boost of 50% for 2 turns.",
			Category:       domain.ItemConsumable,
			ConsumableType: domain.ConsumeBuffDef,
			Value:          50, Duration: 2, BuyPrice: 120, SellPrice: 50, Stackable: true,
		},
		{
			ID: "antidote", Name: "Antidote",
			Description:    "Cures poison status.",
			Category:       domain.ItemConsumable,
			ConsumableType: domain.ConsumeNone,
			Value:          0, Duration: 0, BuyPrice: 30, SellPrice: 12, Stackable: true,
		},

		// =========================================
		// MATERIALS - Monster Drops (Common)
		// =========================================
		{
			ID: "slime_gel", Name: "Slime Gel",
			Description: "Gooey substance from slimes. Used for crafting.",
			Category:    domain.ItemMaterial,
			BuyPrice:    0, SellPrice: 15, Stackable: true,
		},
		{
			ID: "goblin_fang", Name: "Goblin Fang",
			Description: "Sharp fang from a goblin.",
			Category:    domain.ItemMaterial,
			BuyPrice:    0, SellPrice: 20, Stackable: true,
		},
		{
			ID: "bat_wing", Name: "Bat Wing",
			Description: "Leathery wing from a cave bat.",
			Category:    domain.ItemMaterial,
			BuyPrice:    0, SellPrice: 12, Stackable: true,
		},
		{
			ID: "wolf_pelt", Name: "Wolf Pelt",
			Description: "Rugged fur from a wild wolf.",
			Category:    domain.ItemMaterial,
			BuyPrice:    0, SellPrice: 25, Stackable: true,
		},
		{
			ID: "wolf_fang", Name: "Wolf Fang",
			Description: "Sharp fang from a wild wolf.",
			Category:    domain.ItemMaterial,
			BuyPrice:    0, SellPrice: 18, Stackable: true,
		},
		{
			ID: "bone_fragment", Name: "Bone Fragment",
			Description: "Piece of ancient bone.",
			Category:    domain.ItemMaterial,
			BuyPrice:    0, SellPrice: 22, Stackable: true,
		},

		// =========================================
		// MATERIALS - Elemental Essences (Elite drops)
		// =========================================
		{
			ID: "fire_essence", Name: "Fire Essence",
			Description: "Burning core of a fire creature.",
			Category:    domain.ItemMaterial,
			BuyPrice:    0, SellPrice: 80, Stackable: true,
		},
		{
			ID: "water_essence", Name: "Water Essence",
			Description: "Crystallized water magic.",
			Category:    domain.ItemMaterial,
			BuyPrice:    0, SellPrice: 80, Stackable: true,
		},
		{
			ID: "earth_essence", Name: "Earth Essence",
			Description: "Condensed earth energy.",
			Category:    domain.ItemMaterial,
			BuyPrice:    0, SellPrice: 80, Stackable: true,
		},
		{
			ID: "wind_essence", Name: "Wind Essence",
			Description: "Captured wind spirit energy.",
			Category:    domain.ItemMaterial,
			BuyPrice:    0, SellPrice: 80, Stackable: true,
		},
		{
			ID: "ember_stone", Name: "Ember Stone",
			Description: "A stone that glows with inner fire.",
			Category:    domain.ItemMaterial,
			BuyPrice:    0, SellPrice: 60, Stackable: true,
		},
		{
			ID: "aqua_crystal", Name: "Aqua Crystal",
			Description: "A crystal filled with water magic.",
			Category:    domain.ItemMaterial,
			BuyPrice:    0, SellPrice: 60, Stackable: true,
		},
		{
			ID: "stone_core", Name: "Stone Core",
			Description: "Heart of a stone golem.",
			Category:    domain.ItemMaterial,
			BuyPrice:    0, SellPrice: 70, Stackable: true,
		},
		{
			ID: "gale_feather", Name: "Gale Feather",
			Description: "A feather that floats on the wind.",
			Category:    domain.ItemMaterial,
			BuyPrice:    0, SellPrice: 55, Stackable: true,
		},
		{
			ID: "orc_tusk", Name: "Orc Tusk",
			Description: "Massive tusk from an orc warrior.",
			Category:    domain.ItemMaterial,
			BuyPrice:    0, SellPrice: 65, Stackable: true,
		},

		// =========================================
		// MATERIALS - Boss Drops (Legendary)
		// =========================================
		{
			ID: "dragon_scale", Name: "Dragon Scale",
			Description: "Impenetrable scale of an ancient dragon.",
			Category:    domain.ItemMaterial,
			BuyPrice:    0, SellPrice: 300, Stackable: true,
		},
		{
			ID: "dragon_fang", Name: "Dragon Fang",
			Description: "Massive fang capable of piercing anything.",
			Category:    domain.ItemMaterial,
			BuyPrice:    0, SellPrice: 250, Stackable: true,
		},
		{
			ID: "kraken_tentacle", Name: "Kraken Tentacle",
			Description: "Severed tentacle from the deep sea horror.",
			Category:    domain.ItemMaterial,
			BuyPrice:    0, SellPrice: 280, Stackable: true,
		},
		{
			ID: "deep_sea_pearl", Name: "Deep Sea Pearl",
			Description: "A pearl of incredible beauty and power.",
			Category:    domain.ItemMaterial,
			BuyPrice:    0, SellPrice: 350, Stackable: true,
		},
		{
			ID: "titan_heart", Name: "Titan Heart",
			Description: "The core of an earth titan. Immense energy.",
			Category:    domain.ItemMaterial,
			BuyPrice:    0, SellPrice: 400, Stackable: true,
		},
		{
			ID: "adamant_ore", Name: "Adamant Ore",
			Description: "The hardest ore known to exist.",
			Category:    domain.ItemMaterial,
			BuyPrice:    0, SellPrice: 200, Stackable: true,
		},
		{
			ID: "legendary_ore", Name: "Legendary Ore",
			Description: "Mysterious ore radiating with power.",
			Category:    domain.ItemMaterial,
			BuyPrice:    0, SellPrice: 500, Stackable: true,
		},
	}

	for _, i := range items {
		r.items[i.ID] = i
	}
}

// initShopStock initializes shop stock.
// EXACT MATCH: C src/data/shop_stock.c SHOP_DB[] (items only)
func (r *ItemRegistry) initShopStock() {
	// -1 = unlimited, positive = limited stock
	stock := map[string]int{
		"small_potion":  -1,
		"medium_potion": -1,
		"large_potion":  -1,
		"full_restore":  5,
		"atk_elixir":    -1,
		"def_elixir":    -1,
		"power_surge":   3,
		"iron_skin":     3,
		"antidote":      -1,
	}

	for id, s := range stock {
		r.shopStock[id] = s
	}
}

// GetByID returns item by ID.
func (r *ItemRegistry) GetByID(id string) *ItemTemplate {
	return r.items[id]
}

// GetAll returns all items.
func (r *ItemRegistry) GetAll() []*ItemTemplate {
	result := make([]*ItemTemplate, 0, len(r.items))
	for _, i := range r.items {
		result = append(result, i)
	}
	return result
}

// GetConsumables returns all consumable items.
func (r *ItemRegistry) GetConsumables() []*ItemTemplate {
	var result []*ItemTemplate
	for _, i := range r.items {
		if i.Category == domain.ItemConsumable {
			result = append(result, i)
		}
	}
	return result
}

// GetMaterials returns all material items.
func (r *ItemRegistry) GetMaterials() []*ItemTemplate {
	var result []*ItemTemplate
	for _, i := range r.items {
		if i.Category == domain.ItemMaterial {
			result = append(result, i)
		}
	}
	return result
}

// GetShopItems returns items available in shop WITH stock info.
// Returns in order matching C shop_stock.c
func (r *ItemRegistry) GetShopItems() []ShopItemEntry {
	// Order matches C src/data/shop_stock.c
	order := []string{
		"small_potion", "medium_potion", "large_potion", "full_restore",
		"atk_elixir", "def_elixir", "power_surge", "iron_skin", "antidote",
	}

	result := make([]ShopItemEntry, 0, len(order))
	for _, id := range order {
		if i := r.items[id]; i != nil && i.BuyPrice > 0 {
			result = append(result, ShopItemEntry{
				Item:  i,
				Stock: r.shopStock[id],
			})
		}
	}
	return result
}

// GetStock returns current stock level for an item.
// Returns 0 for items not tracked in the shop stock system.
func (r *ItemRegistry) GetStock(id string) int {
	if stock, ok := r.shopStock[id]; ok {
		return stock // Return current stock (-1, 0, or positive count)
	}
	return 0 // Item not in shop stock — treat as unavailable
}

// DecrementStock reduces stock by the specified amount after a purchase.
// Returns true if the decrement succeeded. Unlimited stock (-1) always succeeds.
// NOTE: This method is currently NOT called by ShopService — see documentation.txt.
func (r *ItemRegistry) DecrementStock(id string, amount int) bool {
	if stock, ok := r.shopStock[id]; ok {
		if stock == -1 {
			return true // Unlimited stock — always available, never decremented
		}
		if stock >= amount {
			r.shopStock[id] -= amount // Reduce available stock
			return true               // Purchase succeeded
		}
	}
	return false // Item not found or insufficient stock
}

// ShopItemEntry combines an item template with its current stock level.
// Used by ShopScreen to display items with availability information.
type ShopItemEntry struct {
	Item  *ItemTemplate // Pointer to immutable item definition
	Stock int           // Current stock: -1 = unlimited, 0 = sold out, >0 = remaining
}

// IsSoldOut returns true if item has zero stock remaining.
// Used by shop UI to gray out unavailable items.
func (e ShopItemEntry) IsSoldOut() bool {
	return e.Stock == 0 // Exactly zero — item was available but depleted
}

// IsUnlimited returns true if item has infinite stock.
// Used by shop UI to show "∞" instead of a stock count.
func (e ShopItemEntry) IsUnlimited() bool {
	return e.Stock == -1 // Sentinel value for unlimited availability
}
