package application

import (
	"github.com/tenyom/textrpg-tui/internal/domain"                  // Domain entities and result codes
	"github.com/tenyom/textrpg-tui/internal/infrastructure/registry" // Weapon and Item registries
)

// DefaultSellRatio determines what fraction of buy price a player receives when selling.
// 0.5 means players get 50% of the weapon's buy price back.
// Applied to weapon sales; items use their dedicated SellPrice field.
const DefaultSellRatio = 0.5

// ShopService manages buy/sell transactions between player and shop.
// Validates gold, inventory capacity, and performs atomic transactions.
type ShopService struct {
	player         *domain.Player           // Mutable player reference for gold and inventory
	weaponRegistry *registry.WeaponRegistry // Read-only weapon data source
	itemRegistry   *registry.ItemRegistry   // Read-only item data source
	sellRatio      float64                  // Sell price multiplier (default 0.5)
}

// NewShopService creates shop service with injected dependencies.
// Uses DefaultSellRatio (0.5) for consistent sell pricing.
func NewShopService(
	player *domain.Player,
	weaponReg *registry.WeaponRegistry,
	itemReg *registry.ItemRegistry,
) *ShopService {
	return &ShopService{
		player:         player,           // Store player reference
		weaponRegistry: weaponReg,        // Store weapon data source
		itemRegistry:   itemReg,          // Store item data source
		sellRatio:      DefaultSellRatio, // 0.5 — player gets half the buy price when selling weapons
	}
}

// BuyWeapon purchases a weapon from the shop.
// Validates: weapon exists, player has enough gold, inventory has room.
// Transaction: gold deducted → weapon created from template → added to inventory.
func (ss *ShopService) BuyWeapon(weaponID string) domain.ResultCode {
	// Step 1: Lookup weapon template in registry
	template := ss.weaponRegistry.GetByID(weaponID)
	if template == nil {
		return domain.ResultItemNotFound // Weapon ID doesn't exist in registry
	}

	// Step 2: Validate gold sufficiency
	if ss.player.Gold < template.BuyPrice {
		return domain.ResultNotEnoughGold // Player can't afford this weapon
	}

	// Step 3: Create mutable weapon instance from immutable template
	weapon := template.ToWeapon() // Copies template data into a new Weapon

	// Step 4: Try to add weapon to inventory (may fail if full)
	result := ss.player.Inventory.AddWeapon(weapon)
	if result != domain.ResultSuccess {
		return result // Forward inventory error (ResultInventoryFull)
	}

	// Step 5: Deduct gold — only after successful inventory addition
	// This ordering ensures no gold is lost if inventory is full.
	ss.player.Gold -= template.BuyPrice

	return domain.ResultSuccess // Transaction complete
}

// BuyItem purchases an item from the shop.
// Similar flow to BuyWeapon but creates an Item from ItemTemplate.
// Items are stackable — AddItem handles stacking with existing stacks.
func (ss *ShopService) BuyItem(itemID string) domain.ResultCode {
	// Step 1: Lookup item template in registry
	template := ss.itemRegistry.GetByID(itemID)
	if template == nil {
		return domain.ResultItemNotFound // Item ID doesn't exist in registry
	}

	// Step 2: Validate gold sufficiency
	if ss.player.Gold < template.BuyPrice {
		return domain.ResultNotEnoughGold // Player can't afford this item
	}

	// Step 3: Create mutable item instance from immutable template
	item := template.ToItem() // Copies template data into a new Item

	// Step 4: Try to add item to inventory (1 unit)
	// AddItem handles stacking with existing same-ID items automatically
	result := ss.player.Inventory.AddItem(item, 1)
	if result != domain.ResultSuccess {
		return result // Forward inventory error (ResultInventoryFull)
	}

	// Step 5: Deduct gold — only after successful inventory addition
	ss.player.Gold -= template.BuyPrice

	return domain.ResultSuccess // Transaction complete
}

// SellItem sells an item from inventory.
// Handles both weapons and items. Calculates sell price based on item type:
//   - Weapons: template.BuyPrice * sellRatio (0.5)
//   - Items: item.SellPrice (direct value from item definition)
//
// Returns the gold earned for UI display.
func (ss *ShopService) SellItem(slotIndex int) (int, domain.ResultCode) {
	// Step 1: Validate slot index bounds
	slot := ss.player.Inventory.GetSlot(slotIndex)
	if slot == nil || slot.Type == domain.SlotEmpty {
		return 0, domain.ResultInvalidSlot // Slot doesn't exist or is empty
	}

	// Step 2: Calculate sell price based on slot contents
	var sellValue int
	switch slot.Type {
	case domain.SlotWeapon:
		// Weapon sell price: look up buy price from template, apply sell ratio
		template := ss.weaponRegistry.GetByID(slot.Weapon.ID)
		if template != nil {
			sellValue = int(float64(template.BuyPrice) * ss.sellRatio) // 50% of buy price
		}
	case domain.SlotItem:
		// Item sell price: use the item's dedicated SellPrice field directly
		sellValue = slot.Item.SellPrice
	}

	// Step 3: Remove from inventory (1 unit)
	ss.player.Inventory.RemoveItem(slotIndex, 1)

	// Step 4: Add gold to player balance
	ss.player.AddGold(sellValue)

	return sellValue, domain.ResultSuccess // Return gold earned for UI message
}

// GetShopWeapons returns available weapons from the weapon registry.
// Delegates entirely to WeaponRegistry.GetShopWeapons().
func (ss *ShopService) GetShopWeapons() []registry.WeaponTemplate {
	return ss.weaponRegistry.GetShopWeapons()
}

// GetShopItems returns available items from the item registry.
// Delegates entirely to ItemRegistry.GetShopItems().
func (ss *ShopService) GetShopItems() []registry.ItemTemplate {
	return ss.itemRegistry.GetShopItems()
}
