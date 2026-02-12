package domain

// InventorySlot holds either a weapon or item with quantity.
type InventorySlot struct {
	Type     SlotType
	Weapon   *Weapon
	Item     *Item
	Quantity int
}

// SlotType identifies what a slot contains.
type SlotType int

const (
	SlotEmpty SlotType = iota
	SlotWeapon
	SlotItem
)

// Item represents a consumable or material.
type Item struct {
	ID             string
	Name           string
	Description    string
	Category       ItemCategory
	ConsumableType ConsumableType
	Value          int
	Duration       int
	BuyPrice       int
	SellPrice      int
	Stackable      bool
}

// ItemCategory identifies item type.
type ItemCategory int

const (
	ItemConsumable ItemCategory = iota
	ItemMaterial
)

// ConsumableType identifies consumable effect.
type ConsumableType int

const (
	ConsumeNone ConsumableType = iota
	ConsumeHeal
	ConsumeBuffAtk
	ConsumeBuffDef
	ConsumeFullRestore
)

// Inventory manages player items and weapons.
type Inventory struct {
	Slots    []InventorySlot
	MaxSlots int
}

// NewInventory creates an inventory with given capacity.
func NewInventory(maxSlots int) *Inventory {
	return &Inventory{
		Slots:    make([]InventorySlot, maxSlots),
		MaxSlots: maxSlots,
	}
}

// AddWeapon adds a weapon to inventory.
func (inv *Inventory) AddWeapon(w *Weapon) ResultCode {
	for i := range inv.Slots {
		if inv.Slots[i].Type == SlotEmpty {
			inv.Slots[i] = InventorySlot{
				Type:     SlotWeapon,
				Weapon:   w,
				Quantity: 1,
			}
			return ResultSuccess
		}
	}
	return ResultInventoryFull
}

// AddItem adds items to inventory (stacking if possible).
func (inv *Inventory) AddItem(item *Item, qty int) ResultCode {
	// Try to stack with existing
	if item.Stackable {
		for i := range inv.Slots {
			if inv.Slots[i].Type == SlotItem && inv.Slots[i].Item.ID == item.ID {
				inv.Slots[i].Quantity += qty
				if inv.Slots[i].Quantity > 99 {
					inv.Slots[i].Quantity = 99
				}
				return ResultSuccess
			}
		}
	}

	// Find empty slot
	for i := range inv.Slots {
		if inv.Slots[i].Type == SlotEmpty {
			inv.Slots[i] = InventorySlot{
				Type:     SlotItem,
				Item:     item,
				Quantity: qty,
			}
			return ResultSuccess
		}
	}
	return ResultInventoryFull
}

// RemoveItem removes items from a slot.
func (inv *Inventory) RemoveItem(slotIndex, qty int) ResultCode {
	if slotIndex < 0 || slotIndex >= len(inv.Slots) {
		return ResultInvalidSlot
	}

	slot := &inv.Slots[slotIndex]
	if slot.Type == SlotEmpty {
		return ResultItemNotFound
	}

	slot.Quantity -= qty
	if slot.Quantity <= 0 {
		*slot = InventorySlot{Type: SlotEmpty}
	}
	return ResultSuccess
}

// RemoveWeapon removes weapon from slot.
func (inv *Inventory) RemoveWeapon(slotIndex int) (*Weapon, ResultCode) {
	if slotIndex < 0 || slotIndex >= len(inv.Slots) {
		return nil, ResultInvalidSlot
	}

	slot := &inv.Slots[slotIndex]
	if slot.Type != SlotWeapon {
		return nil, ResultItemNotFound
	}

	w := slot.Weapon
	*slot = InventorySlot{Type: SlotEmpty}
	return w, ResultSuccess
}

// GetSlot returns slot at index.
func (inv *Inventory) GetSlot(index int) *InventorySlot {
	if index < 0 || index >= len(inv.Slots) {
		return nil
	}
	return &inv.Slots[index]
}

// UsedSlots returns number of occupied slots.
func (inv *Inventory) UsedSlots() int {
	count := 0
	for _, slot := range inv.Slots {
		if slot.Type != SlotEmpty {
			count++
		}
	}
	return count
}

// GetWeapons returns all weapons in inventory.
func (inv *Inventory) GetWeapons() []*Weapon {
	var weapons []*Weapon
	for _, slot := range inv.Slots {
		if slot.Type == SlotWeapon {
			weapons = append(weapons, slot.Weapon)
		}
	}
	return weapons
}

// GetItems returns all items in inventory.
func (inv *Inventory) GetItems() []*InventorySlot {
	var items []*InventorySlot
	for i := range inv.Slots {
		if inv.Slots[i].Type == SlotItem {
			items = append(items, &inv.Slots[i])
		}
	}
	return items
}

// GetConsumables returns consumable items only.
func (inv *Inventory) GetConsumables() []*InventorySlot {
	var items []*InventorySlot
	for i := range inv.Slots {
		if inv.Slots[i].Type == SlotItem && inv.Slots[i].Item.Category == ItemConsumable {
			items = append(items, &inv.Slots[i])
		}
	}
	return items
}

// FindItemSlot finds slot index containing item ID.
func (inv *Inventory) FindItemSlot(itemID string) int {
	for i, slot := range inv.Slots {
		if slot.Type == SlotItem && slot.Item.ID == itemID {
			return i
		}
	}
	return -1
}

// GetOccupiedSlots returns indices of all occupied slots.
func (inv *Inventory) GetOccupiedSlots() []int {
	var slots []int
	for i, slot := range inv.Slots {
		if slot.Type != SlotEmpty {
			slots = append(slots, i)
		}
	}
	return slots
}

// CountItem returns how many of an item the player has.
func (inv *Inventory) CountItem(itemID string) int {
	for _, slot := range inv.Slots {
		if slot.Type == SlotItem && slot.Item.ID == itemID {
			return slot.Quantity
		}
	}
	return 0
}

// RemoveItemByID removes items by ID across slots.
func (inv *Inventory) RemoveItemByID(itemID string, qty int) ResultCode {
	remaining := qty
	for i := range inv.Slots {
		if remaining <= 0 {
			break
		}
		slot := &inv.Slots[i]
		if slot.Type == SlotItem && slot.Item.ID == itemID {
			if slot.Quantity <= remaining {
				remaining -= slot.Quantity
				*slot = InventorySlot{Type: SlotEmpty}
			} else {
				slot.Quantity -= remaining
				remaining = 0
			}
		}
	}
	if remaining > 0 {
		return ResultItemNotFound
	}
	return ResultSuccess
}

// MaterialInfo represents a material with quantity for display.
type MaterialInfo struct {
	Name     string
	ID       string
	Quantity int
}

// GetMaterials returns all material items for display.
func (inv *Inventory) GetMaterials() []MaterialInfo {
	var materials []MaterialInfo
	for _, slot := range inv.Slots {
		if slot.Type == SlotItem && slot.Item.Category == ItemMaterial {
			materials = append(materials, MaterialInfo{
				Name:     slot.Item.Name,
				ID:       slot.Item.ID,
				Quantity: slot.Quantity,
			})
		}
	}
	return materials
}

// MaxInventorySlots is the default inventory size.
const MaxInventorySlots = 20
