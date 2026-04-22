// Package registry provides read-only data registries for game entities.
// All weapon data EXACTLY matches C src/data/weapons.c for feature parity.
package registry

import "github.com/tenyom/textrpg-tui/internal/domain" // Domain types for WeaponType, Element, Weapon

// WeaponTemplate is an immutable weapon definition loaded at startup.
// Matches C src/data/weapons.c WeaponTemplate struct.
// Templates are never modified — mutable domain.Weapon instances are
// created via ToWeapon() when purchased or given to the player.
type WeaponTemplate struct {
	ID          string            // Unique key (e.g., "iron_sword", "fire_blade")
	Name        string            // Display name for shop/inventory UI
	Description string            // Flavor text for weapon details
	Type        domain.WeaponType // Weapon category (Sword, Axe, Staff, Bow, Dagger)
	Element     domain.Element    // Elemental affinity for damage modifiers
	BaseDamage  int               // Base damage before tier multiplication
	BuyPrice    int               // Shop purchase cost in gold (0 = not purchasable)
	SellPrice   int               // Gold received when selling this weapon
	MaxTier     int               // Maximum upgrade tier (typically 5)
}

// ToWeapon creates a mutable domain.Weapon from this immutable template.
// New weapons always start at Tier 1 (base stats, no upgrade bonuses).
// Called by ShopService.BuyWeapon() and giveStartingItems().
func (t *WeaponTemplate) ToWeapon() *domain.Weapon {
	return &domain.Weapon{
		ID:          t.ID,          // Copy identifier
		Name:        t.Name,        // Copy display name
		Description: t.Description, // Copy flavor text
		Type:        t.Type,        // Copy weapon category
		Element:     t.Element,     // Copy elemental affinity
		BaseDamage:  t.BaseDamage,  // Copy base damage value
		Tier:        1,             // All new weapons start at Tier 1
		MaxTier:     t.MaxTier,     // Copy maximum upgrade tier
	}
}

// WeaponRegistry is the read-only data store for all weapon definitions.
// Supports lookup by ID and shop stock management.
type WeaponRegistry struct {
	weapons   map[string]*WeaponTemplate // All weapons indexed by ID for O(1) lookup
	shopStock map[string]int             // Shop availability: -1 = unlimited, 0 = sold out, >0 = remaining
}

// NewWeaponRegistry creates and initializes the weapon registry.
// Loads all 18 weapon templates and sets initial shop stock levels.
func NewWeaponRegistry() *WeaponRegistry {
	r := &WeaponRegistry{
		weapons:   make(map[string]*WeaponTemplate), // Initialize ID lookup map
		shopStock: make(map[string]int),             // Initialize stock tracker
	}
	r.loadWeapons()   // Populate 18 weapon definitions
	r.initShopStock() // Set initial shop stock levels
	return r
}

// loadWeapons initializes all weapon data.
// EXACT MATCH: C src/data/weapons.c WEAPON_DB[]
func (r *WeaponRegistry) loadWeapons() {
	weapons := []*WeaponTemplate{
		// =========================================
		// STARTER / BASIC WEAPONS
		// =========================================
		{
			ID: "fists", Name: "Fists",
			Description: "Your bare hands. Better find a real weapon.",
			Type:        domain.WeaponFist, Element: domain.ElementNone,
			BaseDamage: 5, BuyPrice: 0, SellPrice: 0, MaxTier: 1,
		},
		{
			ID: "rusty_sword", Name: "Rusty Sword",
			Description: "An old sword, covered in rust but still usable.",
			Type:        domain.WeaponSword, Element: domain.ElementNone,
			BaseDamage: 8, BuyPrice: 0, SellPrice: 15, MaxTier: 1,
		},

		// =========================================
		// SWORDS - Balanced weapons
		// =========================================
		{
			ID: "iron_sword", Name: "Iron Sword",
			Description: "A sturdy iron blade. Reliable and upgradeable.",
			Type:        domain.WeaponSword, Element: domain.ElementNone,
			BaseDamage: 15, BuyPrice: 100, SellPrice: 50, MaxTier: 5,
		},
		{
			ID: "steel_sword", Name: "Steel Sword",
			Description: "Forged from high-quality steel. Sharp and durable.",
			Type:        domain.WeaponSword, Element: domain.ElementNone,
			BaseDamage: 22, BuyPrice: 250, SellPrice: 125, MaxTier: 5,
		},
		{
			ID: "fire_blade", Name: "Fire Blade",
			Description: "A sword imbued with flames. Effective against wind.",
			Type:        domain.WeaponSword, Element: domain.ElementFire,
			BaseDamage: 20, BuyPrice: 300, SellPrice: 150, MaxTier: 5,
		},
		{
			ID: "aqua_saber", Name: "Aqua Saber",
			Description: "A blade of crystallized water. Strong against fire.",
			Type:        domain.WeaponSword, Element: domain.ElementWater,
			BaseDamage: 20, BuyPrice: 300, SellPrice: 150, MaxTier: 5,
		},
		{
			ID: "earth_cleaver", Name: "Earth Cleaver",
			Description: "Heavy blade infused with earth magic.",
			Type:        domain.WeaponSword, Element: domain.ElementEarth,
			BaseDamage: 20, BuyPrice: 300, SellPrice: 150, MaxTier: 5,
		},
		{
			ID: "wind_cutter", Name: "Wind Cutter",
			Description: "Swift blade that cuts like the wind.",
			Type:        domain.WeaponSword, Element: domain.ElementWind,
			BaseDamage: 18, BuyPrice: 280, SellPrice: 140, MaxTier: 5,
		},

		// =========================================
		// AXES - High damage, slower
		// =========================================
		{
			ID: "iron_axe", Name: "Iron Axe",
			Description: "A heavy axe. Deals massive damage.",
			Type:        domain.WeaponAxe, Element: domain.ElementNone,
			BaseDamage: 20, BuyPrice: 150, SellPrice: 75, MaxTier: 5,
		},
		{
			ID: "battle_axe", Name: "Battle Axe",
			Description: "A warrior's axe, forged for combat.",
			Type:        domain.WeaponAxe, Element: domain.ElementNone,
			BaseDamage: 28, BuyPrice: 0, SellPrice: 100, MaxTier: 5,
		},

		// =========================================
		// STAVES - Magic-focused
		// =========================================
		{
			ID: "wooden_staff", Name: "Wooden Staff",
			Description: "A simple staff. Good for beginners.",
			Type:        domain.WeaponStaff, Element: domain.ElementNone,
			BaseDamage: 10, BuyPrice: 80, SellPrice: 40, MaxTier: 3,
		},
		{
			ID: "fire_staff", Name: "Fire Staff",
			Description: "Channels the power of flames.",
			Type:        domain.WeaponStaff, Element: domain.ElementFire,
			BaseDamage: 18, BuyPrice: 280, SellPrice: 140, MaxTier: 5,
		},
		{
			ID: "ice_staff", Name: "Ice Staff",
			Description: "Frozen magic flows through this staff.",
			Type:        domain.WeaponStaff, Element: domain.ElementWater,
			BaseDamage: 18, BuyPrice: 280, SellPrice: 140, MaxTier: 5,
		},

		// =========================================
		// DAGGERS - Fast, lower damage
		// =========================================
		{
			ID: "iron_dagger", Name: "Iron Dagger",
			Description: "Quick and deadly. Perfect for swift strikes.",
			Type:        domain.WeaponDagger, Element: domain.ElementNone,
			BaseDamage: 12, BuyPrice: 80, SellPrice: 40, MaxTier: 5,
		},
		{
			ID: "assassin_blade", Name: "Assassin Blade",
			Description: "A shadowy dagger. Whispers of death.",
			Type:        domain.WeaponDagger, Element: domain.ElementWind,
			BaseDamage: 16, BuyPrice: 220, SellPrice: 110, MaxTier: 5,
		},

		// =========================================
		// BOWS - Ranged weapons
		// =========================================
		{
			ID: "hunting_bow", Name: "Hunting Bow",
			Description: "A reliable bow for hunting and combat.",
			Type:        domain.WeaponBow, Element: domain.ElementNone,
			BaseDamage: 14, BuyPrice: 120, SellPrice: 60, MaxTier: 5,
		},
		{
			ID: "gale_bow", Name: "Gale Bow",
			Description: "Arrows fly swift as the wind itself.",
			Type:        domain.WeaponBow, Element: domain.ElementWind,
			BaseDamage: 18, BuyPrice: 260, SellPrice: 130, MaxTier: 5,
		},

		// =========================================
		// LEGENDARY WEAPONS (craft only)
		// =========================================
		{
			ID: "dragon_slayer", Name: "Dragon Slayer",
			Description: "Legendary blade forged from dragon scales.",
			Type:        domain.WeaponSword, Element: domain.ElementFire,
			BaseDamage: 45, BuyPrice: 0, SellPrice: 500, MaxTier: 5,
		},
		{
			ID: "titan_hammer", Name: "Titan's Hammer",
			Description: "Weapon of the earth titans. Immense power.",
			Type:        domain.WeaponAxe, Element: domain.ElementEarth,
			BaseDamage: 50, BuyPrice: 0, SellPrice: 600, MaxTier: 5,
		},
	}

	for _, w := range weapons {
		r.weapons[w.ID] = w
	}
}

// initShopStock initializes shop stock.
// EXACT MATCH: C src/data/shop_stock.c SHOP_DB[] (weapons only)
func (r *WeaponRegistry) initShopStock() {
	// -1 = unlimited, positive = limited stock
	stock := map[string]int{
		"iron_sword":     -1,
		"steel_sword":    -1,
		"fire_blade":     1,
		"aqua_saber":     1,
		"earth_cleaver":  1,
		"wind_cutter":    1,
		"iron_axe":       -1,
		"wooden_staff":   -1,
		"fire_staff":     1,
		"ice_staff":      1,
		"iron_dagger":    -1,
		"assassin_blade": 1,
		"hunting_bow":    -1,
		"gale_bow":       1,
	}

	for id, s := range stock {
		r.shopStock[id] = s
	}
}

// GetByID returns weapon by ID.
func (r *WeaponRegistry) GetByID(id string) *WeaponTemplate {
	return r.weapons[id]
}

// GetAll returns all weapons.
func (r *WeaponRegistry) GetAll() []*WeaponTemplate {
	result := make([]*WeaponTemplate, 0, len(r.weapons))
	for _, w := range r.weapons {
		result = append(result, w)
	}
	return result
}

// GetShopWeapons returns weapons available in shop WITH stock info.
// Returns in order matching C shop_stock.c
func (r *WeaponRegistry) GetShopWeapons() []ShopWeaponEntry {
	// Order matches C src/data/shop_stock.c
	order := []string{
		"iron_sword", "steel_sword", "fire_blade", "aqua_saber",
		"earth_cleaver", "wind_cutter", "iron_axe", "wooden_staff",
		"fire_staff", "ice_staff", "iron_dagger", "assassin_blade",
		"hunting_bow", "gale_bow",
	}

	result := make([]ShopWeaponEntry, 0, len(order))
	for _, id := range order {
		if w := r.weapons[id]; w != nil && w.BuyPrice > 0 {
			result = append(result, ShopWeaponEntry{
				Weapon: w,
				Stock:  r.shopStock[id],
			})
		}
	}
	return result
}

// GetStock returns stock for weapon (0 = sold out, -1 = unlimited).
func (r *WeaponRegistry) GetStock(id string) int {
	if stock, ok := r.shopStock[id]; ok {
		return stock
	}
	return 0
}

// DecrementStock reduces stock by 1 (for purchase).
func (r *WeaponRegistry) DecrementStock(id string) bool {
	if stock, ok := r.shopStock[id]; ok {
		if stock == -1 {
			return true // Unlimited
		}
		if stock > 0 {
			r.shopStock[id]--
			return true
		}
	}
	return false // Sold out
}

// ShopWeaponEntry represents a weapon in the shop.
type ShopWeaponEntry struct {
	Weapon *WeaponTemplate
	Stock  int // -1 = unlimited, 0 = sold out
}

// IsSoldOut returns true if weapon is sold out.
func (e ShopWeaponEntry) IsSoldOut() bool {
	return e.Stock == 0
}

// IsUnlimited returns true if weapon has unlimited stock.
func (e ShopWeaponEntry) IsUnlimited() bool {
	return e.Stock == -1
}
