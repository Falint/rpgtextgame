package domain

// InventorySlot holds either a weapon or item with quantity.
// Uses a discriminated union pattern: the Type field indicates which
// pointer (Weapon or Item) is valid. Only one is non-nil at a time.
type InventorySlot struct {
	Type     SlotType // Discriminator: SlotEmpty, SlotWeapon, or SlotItem
	Weapon   *Weapon  // Non-nil only when Type == SlotWeapon
	Item     *Item    // Non-nil only when Type == SlotItem
	Quantity int      // Count: always 1 for weapons, variable for stackable items
}

// SlotType identifies what a slot contains.
// Used as a discriminator for the InventorySlot union type.
type SlotType int

// Slot type constants using iota for auto-incrementing enum.
const (
	SlotEmpty  SlotType = iota // Slot contains nothing — available for storage
	SlotWeapon                 // Slot contains a Weapon (Quantity always 1)
	SlotItem                   // Slot contains an Item (Quantity >= 1)
)

// Inventory capacity and stacking limits.
// Named constants prevent magic numbers throughout inventory operations.
const (
	MaxInventorySlots = 20 // Default inventory size (matches C version)
	MaxItemStackSize  = 99 // Maximum quantity per stackable item slot
)

// Item represents a consumable or material.
// Items are created from ItemTemplate definitions in the registry.
// Consumables are destroyed on use; materials are consumed in upgrades.
type Item struct {
	ID             string         // Unique identifier (e.g., "small_potion")
	Name           string         // Display name (e.g., "Small Potion")
	Description    string         // Flavor text shown in shop/inventory
	Category       ItemCategory   // Consumable or Material — determines usability
	ConsumableType ConsumableType // Effect type (Heal, BuffAtk, etc.) — only for consumables
	Value          int            // Effect magnitude: heal amount, buff percentage, etc.
	Duration       int            // Buff duration in turns (0 for non-buff items)
	BuyPrice       int            // Shop purchase cost in gold (0 = not buyable)
	SellPrice      int            // Base sell value in gold
	Stackable      bool           // Whether multiple units can share one inventory slot
}

// ItemCategory identifies item type.
// Determines whether an item can be used (consumed) or is a crafting ingredient.
type ItemCategory int

// Item category constants.
const (
	ItemConsumable ItemCategory = iota // Can be used/consumed for effects (potions, elixirs)
	ItemMaterial                       // Crafting ingredient used in weapon upgrades
)

// ConsumableType identifies consumable effect.
// Dispatched by ItemService.UseConsumable() to apply the correct effect.
type ConsumableType int

// Consumable type constants define all possible consumable effects.
const (
	ConsumeNone        ConsumableType = iota // No effect — placeholder (e.g., Antidote without poison system)
	ConsumeHeal                              // Restores HP by Item.Value amount
	ConsumeBuffAtk                           // Adds attack buff: +Value% ATK for Duration turns
	ConsumeBuffDef                           // Adds defense buff: +Value% DEF for Duration turns
	ConsumeFullRestore                       // Fully restores HP to MaxHP
)

// Inventory manages player items and weapons.
// Uses a fixed-size slot array for O(1) index access and bounded memory usage.
type Inventory struct {
	Slots    []InventorySlot // Fixed-length array of inventory positions
	MaxSlots int             // Capacity (typically MaxInventorySlots = 20)
}

// NewInventory creates an inventory with given capacity.
// All slots start as SlotEmpty with zero-value structs.
func NewInventory(maxSlots int) *Inventory {
	return &Inventory{
		Slots:    make([]InventorySlot, maxSlots), // Pre-allocate all slots
		MaxSlots: maxSlots,                        // Store capacity for reference
	}
}

// AddWeapon adds a weapon to inventory.
// Scans for the first empty slot. Weapons are never stackable (always occupy one slot).
// Returns ResultInventoryFull if no empty slot is found.
func (inv *Inventory) AddWeapon(w *Weapon) ResultCode {
	for i := range inv.Slots { // Linear scan for first empty slot
		if inv.Slots[i].Type == SlotEmpty { // Found empty slot
			inv.Slots[i] = InventorySlot{
				Type:     SlotWeapon, // Mark slot as containing a weapon
				Weapon:   w,          // Store weapon pointer
				Quantity: 1,          // Weapons always have quantity 1
			}
			return ResultSuccess // Weapon added successfully
		}
	}
	return ResultInventoryFull // All slots occupied
}

// AddItem adds items to inventory (stacking if possible).
// First attempts to stack with an existing slot of the same item ID.
// If no existing stack is found, allocates a new empty slot.
// Stacking is capped at MaxItemStackSize (99).
func (inv *Inventory) AddItem(item *Item, qty int) ResultCode {
	// Try to stack with existing slot of same item ID
	if item.Stackable { // Materials and consumables are typically stackable
		for i := range inv.Slots {
			// Find matching non-empty item slot with same ID
			if inv.Slots[i].Type == SlotItem && inv.Slots[i].Item.ID == item.ID {
				inv.Slots[i].Quantity += qty // Increase quantity in existing stack
				// Enforce maximum stack size to prevent unbounded accumulation
				if inv.Slots[i].Quantity > MaxItemStackSize {
					inv.Slots[i].Quantity = MaxItemStackSize // Cap at 99
				}
				return ResultSuccess // Stacked successfully
			}
		}
	}

	// Find empty slot — necessary if item isn't stackable or no existing stack found
	for i := range inv.Slots {
		if inv.Slots[i].Type == SlotEmpty { // Found available slot
			inv.Slots[i] = InventorySlot{
				Type:     SlotItem, // Mark slot as containing an item
				Item:     item,     // Store item pointer
				Quantity: qty,      // Set initial quantity
			}
			return ResultSuccess // Item placed in new slot
		}
	}
	return ResultInventoryFull // No empty slots available
}

// RemoveItem removes items from a slot by index.
// Decrements quantity by the specified amount. If quantity reaches zero
// or below, the slot is cleared to SlotEmpty (fully removing the item).
func (inv *Inventory) RemoveItem(slotIndex, qty int) ResultCode {
	// Bounds check to prevent index-out-of-range panic
	if slotIndex < 0 || slotIndex >= len(inv.Slots) {
		return ResultInvalidSlot
	}

	slot := &inv.Slots[slotIndex] // Get pointer to avoid copying
	if slot.Type == SlotEmpty {   // Cannot remove from empty slot
		return ResultItemNotFound
	}

	slot.Quantity -= qty    // Decrement quantity by requested amount
	if slot.Quantity <= 0 { // Item stack exhausted
		*slot = InventorySlot{Type: SlotEmpty} // Clear slot to empty state
	}
	return ResultSuccess
}

// RemoveWeapon removes weapon from slot.
// Returns the weapon pointer for use by the caller (e.g., equipping).
// Clears the slot after removal.
func (inv *Inventory) RemoveWeapon(slotIndex int) (*Weapon, ResultCode) {
	// Bounds check
	if slotIndex < 0 || slotIndex >= len(inv.Slots) {
		return nil, ResultInvalidSlot
	}

	slot := &inv.Slots[slotIndex] // Get pointer to slot
	if slot.Type != SlotWeapon {  // Verify slot actually contains a weapon
		return nil, ResultItemNotFound
	}

	w := slot.Weapon                       // Capture weapon pointer before clearing
	*slot = InventorySlot{Type: SlotEmpty} // Clear slot
	return w, ResultSuccess                // Return weapon to caller
}

// GetSlot returns slot at index.
// Returns nil for out-of-bounds indices, preventing panics.
func (inv *Inventory) GetSlot(index int) *InventorySlot {
	if index < 0 || index >= len(inv.Slots) {
		return nil // Out of bounds — return nil safely
	}
	return &inv.Slots[index] // Return pointer for mutation access
}

// UsedSlots returns number of occupied slots.
// Used by inventory screen to display "Total Items: N/20".
func (inv *Inventory) UsedSlots() int {
	count := 0
	for _, slot := range inv.Slots { // Iterate all slots
		if slot.Type != SlotEmpty { // Count non-empty slots
			count++
		}
	}
	return count
}

// GetWeapons returns all weapons in inventory.
// Used by equip screen to list equippable weapons.
// Returns a slice of weapon pointers (not copies) for display.
func (inv *Inventory) GetWeapons() []*Weapon {
	var weapons []*Weapon
	for _, slot := range inv.Slots {
		if slot.Type == SlotWeapon { // Filter weapon slots
			weapons = append(weapons, slot.Weapon) // Collect weapon pointers
		}
	}
	return weapons
}

// GetItems returns all items in inventory.
// Returns pointers to inventory slots (not copies) for both display and mutation.
func (inv *Inventory) GetItems() []*InventorySlot {
	var items []*InventorySlot
	for i := range inv.Slots { // Use index-based iteration for pointer stability
		if inv.Slots[i].Type == SlotItem { // Filter item slots
			items = append(items, &inv.Slots[i]) // Append pointer to slot
		}
	}
	return items
}

// GetConsumables returns consumable items only.
// Used by battle screen and inventory screen to list usable items.
// Filters both by SlotItem type AND ItemConsumable category.
func (inv *Inventory) GetConsumables() []*InventorySlot {
	var items []*InventorySlot
	for i := range inv.Slots {
		// Double filter: must be an item AND must be consumable category
		if inv.Slots[i].Type == SlotItem && inv.Slots[i].Item.Category == ItemConsumable {
			items = append(items, &inv.Slots[i])
		}
	}
	return items
}

// FindItemSlot finds slot index containing item ID.
// Returns -1 if no slot contains the specified item.
// Used for lookup operations that need the actual slot index.
func (inv *Inventory) FindItemSlot(itemID string) int {
	for i, slot := range inv.Slots {
		if slot.Type == SlotItem && slot.Item.ID == itemID { // Match by item ID
			return i // Return the first matching slot index
		}
	}
	return -1 // Item not found in any slot
}

// GetOccupiedSlots returns indices of all occupied slots.
// Used by shop sell screen to list all sellable inventory items.
func (inv *Inventory) GetOccupiedSlots() []int {
	var slots []int
	for i, slot := range inv.Slots {
		if slot.Type != SlotEmpty { // Include both weapons and items
			slots = append(slots, i) // Store slot index
		}
	}
	return slots
}

// CountItem returns how many of an item the player has.
// Searches by item ID and returns the quantity from the matching slot.
// Used by upgrade screen to check material requirements.
func (inv *Inventory) CountItem(itemID string) int {
	for _, slot := range inv.Slots {
		if slot.Type == SlotItem && slot.Item.ID == itemID { // Match by item ID
			return slot.Quantity // Return quantity from first matching slot
		}
	}
	return 0 // Item not found — quantity is zero
}

// RemoveItemByID removes items by ID across slots.
// Handles the case where the requested quantity spans multiple slots
// (though in practice, stackable items should be in a single slot).
// Returns ResultItemNotFound if total available quantity is insufficient.
func (inv *Inventory) RemoveItemByID(itemID string, qty int) ResultCode {
	remaining := qty // Track how many units still need to be removed
	for i := range inv.Slots {
		if remaining <= 0 { // All units removed successfully
			break
		}
		slot := &inv.Slots[i]
		// Find slots containing the specified item
		if slot.Type == SlotItem && slot.Item.ID == itemID {
			if slot.Quantity <= remaining {
				// Entire slot consumed — remove all and clear slot
				remaining -= slot.Quantity
				*slot = InventorySlot{Type: SlotEmpty} // Clear to empty
			} else {
				// Partial consumption — decrement quantity
				slot.Quantity -= remaining
				remaining = 0 // All units accounted for
			}
		}
	}
	if remaining > 0 { // Couldn't remove enough units
		return ResultItemNotFound
	}
	return ResultSuccess // All requested units removed
}

// MaterialInfo represents a material with quantity for display.
// Simple value type used to decouple display logic from inventory slot internals.
type MaterialInfo struct {
	Name     string // Display name of the material
	ID       string // Item ID for cross-referencing with upgrade recipes
	Quantity int    // Count owned by the player
}

// GetMaterials returns all material items for display.
// Used by upgrade screen and inventory screen to show crafting ingredients.
func (inv *Inventory) GetMaterials() []MaterialInfo {
	var materials []MaterialInfo
	for _, slot := range inv.Slots {
		// Filter for item slots with Material category
		if slot.Type == SlotItem && slot.Item.Category == ItemMaterial {
			materials = append(materials, MaterialInfo{
				Name:     slot.Item.Name, // Copy display name
				ID:       slot.Item.ID,   // Copy ID for recipe lookups
				Quantity: slot.Quantity,  // Copy current quantity
			})
		}
	}
	return materials
}
