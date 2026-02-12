package application

import (
	"github.com/tenyom/textrpg-tui/internal/domain"
	"github.com/tenyom/textrpg-tui/internal/infrastructure/registry"
)

// ShopService handles buying and selling.
type ShopService struct {
	player    *domain.Player
	weapons   *registry.WeaponRegistry
	items     *registry.ItemRegistry
	sellRatio float64
}

// NewShopService creates a shop service.
func NewShopService(player *domain.Player, weapons *registry.WeaponRegistry, items *registry.ItemRegistry) *ShopService {
	return &ShopService{
		player:    player,
		weapons:   weapons,
		items:     items,
		sellRatio: 0.5,
	}
}

// GetShopWeapons returns weapons available for purchase with stock info.
func (s *ShopService) GetShopWeapons() []registry.ShopWeaponEntry {
	return s.weapons.GetShopWeapons()
}

// GetShopItems returns items available for purchase with stock info.
func (s *ShopService) GetShopItems() []registry.ShopItemEntry {
	return s.items.GetShopItems()
}

// BuyWeapon purchases a weapon.
func (s *ShopService) BuyWeapon(weaponID string) domain.ResultCode {
	template := s.weapons.GetByID(weaponID)
	if template == nil {
		return domain.ResultItemNotFound
	}

	if s.player.Gold < template.BuyPrice {
		return domain.ResultNotEnoughGold
	}

	weapon := template.ToWeapon()
	result := s.player.Inventory.AddWeapon(weapon)
	if result != domain.ResultSuccess {
		return result
	}

	s.player.Gold -= template.BuyPrice
	return domain.ResultSuccess
}

// BuyItem purchases an item.
func (s *ShopService) BuyItem(itemID string, quantity int) domain.ResultCode {
	template := s.items.GetByID(itemID)
	if template == nil {
		return domain.ResultItemNotFound
	}

	totalCost := template.BuyPrice * quantity
	if s.player.Gold < totalCost {
		return domain.ResultNotEnoughGold
	}

	item := &domain.Item{
		ID:             template.ID,
		Name:           template.Name,
		Description:    template.Description,
		Category:       domain.ItemCategory(template.Category),
		ConsumableType: domain.ConsumableType(template.ConsumableType),
		Value:          template.Value,
		Duration:       template.Duration,
		BuyPrice:       template.BuyPrice,
		SellPrice:      template.SellPrice,
		Stackable:      template.Stackable,
	}

	result := s.player.Inventory.AddItem(item, quantity)
	if result != domain.ResultSuccess {
		return result
	}

	s.player.Gold -= totalCost
	return domain.ResultSuccess
}

// SellItem sells items from inventory.
func (s *ShopService) SellItem(slotIndex int, quantity int) domain.ResultCode {
	slot := s.player.Inventory.GetSlot(slotIndex)
	if slot == nil || slot.Type == domain.SlotEmpty {
		return domain.ResultInvalidSlot
	}

	var sellPrice int
	if slot.Type == domain.SlotWeapon {
		// Sell weapon
		weapon := slot.Weapon
		template := s.weapons.GetByID(weapon.ID)
		if template != nil {
			sellPrice = int(float64(template.SellPrice) * s.sellRatio)
		}
		s.player.Inventory.RemoveWeapon(slotIndex)
	} else if slot.Type == domain.SlotItem {
		// Sell items
		item := slot.Item
		if quantity > slot.Quantity {
			quantity = slot.Quantity
		}
		sellPrice = item.SellPrice * quantity
		s.player.Inventory.RemoveItem(slotIndex, quantity)
	}

	s.player.Gold += sellPrice
	return domain.ResultSuccess
}

// GetPlayerGold returns player's current gold.
func (s *ShopService) GetPlayerGold() int {
	return s.player.Gold
}
