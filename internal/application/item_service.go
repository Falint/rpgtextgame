package application

import (
	"fmt" // Sprintf for building result messages

	"github.com/tenyom/textrpg-tui/internal/domain" // Domain types for player, item, buff operations
)

// UseResult encapsulates the outcome of using a consumable item.
// Separates success/failure from messages, allowing callers to handle
// both UI feedback (Message/Error) and control flow (Success) independently.
type UseResult struct {
	Success bool   // Whether the effect was applied — true means item should be consumed
	Message string // Human-readable outcome message for successful use
	Error   string // Human-readable reason for failure (empty on success)
}

// ItemService processes consumable item effects.
// DESIGN: This service is the Single Source of Truth for item effects.
// It is shared between BattleService and InventoryScreen to ensure:
//   - Identical validation (e.g., "HP already full" check)
//   - Identical effect application (e.g., exact heal amount)
//   - Consistent buff behavior (duration, percentage values)
//
// IMPORTANT: This service does NOT remove items from inventory.
// The caller handles removal because different contexts use different
// removal strategies:
//   - BattleService: removes by slot index
//   - InventoryScreen: removes by consumable-list index mapping
type ItemService struct {
	// No dependencies — pure item-effect logic.
	// Could accept a logger in the future for analytics.
}

// NewItemService creates item service.
// No dependencies needed — this is a pure logic service.
func NewItemService() *ItemService {
	return &ItemService{} // No fields to initialize
}

// UseConsumable processes a consumable item's effect on the player.
// Dispatches to specific handlers based on ConsumableType.
// Each handler performs validation and effect application.
// Returns UseResult for UI feedback — does NOT modify inventory.
func (is *ItemService) UseConsumable(player *domain.Player, item *domain.Item) UseResult {
	// Guard: verify the item is actually a consumable
	if item.Category != domain.ItemConsumable {
		return UseResult{
			Success: false,
			Error:   "This item cannot be used.", // Non-consumable items (materials) can't be used
		}
	}

	// Dispatch to the appropriate handler based on consumable type
	switch item.ConsumableType {
	case domain.ConsumeHeal:
		return is.applyHeal(player, item) // Heal by item.Value HP

	case domain.ConsumeFullRestore:
		return is.applyFullRestore(player) // Restore to MaxHP

	case domain.ConsumeBuffAtk:
		return is.applyBuff(player, domain.BuffAttack, item) // ATK buff

	case domain.ConsumeBuffDef:
		return is.applyBuff(player, domain.BuffDefense, item) // DEF buff

	default:
		// ConsumeNone or any future unhandled type
		return UseResult{
			Success: false,
			Error:   "This item cannot be used.", // No effect handler defined
		}
	}
}

// applyHeal processes healing items (Small/Medium/Large Potion).
// Validates that the player isn't already at full HP before healing.
// Heal amount is capped at MaxHP by the player's Heal() method.
func (is *ItemService) applyHeal(player *domain.Player, item *domain.Item) UseResult {
	// Pre-condition check: healing at full HP is wasteful
	if player.CurrentHP >= player.MaxHP {
		return UseResult{
			Success: false,
			Error:   "HP is already full!", // Inform UI that item was not consumed
		}
	}

	// Record HP before healing for accurate "healed X HP" message
	oldHP := player.CurrentHP
	player.Heal(item.Value) // Apply healing (capped at MaxHP internally)

	// Calculate actual HP restored (may be less than item.Value if near max)
	healed := player.CurrentHP - oldHP

	return UseResult{
		Success: true,
		Message: fmt.Sprintf("Used %s! Healed %d HP. (HP: %d/%d)",
			item.Name, healed, player.CurrentHP, player.MaxHP),
	}
}

// applyFullRestore processes Full Restore items.
// Restores HP to maximum. Validates that HP isn't already full.
func (is *ItemService) applyFullRestore(player *domain.Player) UseResult {
	// Pre-condition check: same as regular heal
	if player.CurrentHP >= player.MaxHP {
		return UseResult{
			Success: false,
			Error:   "HP is already full!", // Item not consumed
		}
	}

	// Calculate healing amount for message before modifying state
	healed := player.MaxHP - player.CurrentHP
	player.CurrentHP = player.MaxHP // Set HP directly to maximum

	return UseResult{
		Success: true,
		Message: fmt.Sprintf("Full Restore! Healed %d HP. (HP: %d/%d)",
			healed, player.CurrentHP, player.MaxHP),
	}
}

// applyBuff processes buff items (Attack/Defense Elixirs, Power Surge, Iron Skin).
// Buffs stack multiplicatively with existing buffs of the same type.
// Duration and value are taken from the item's Duration and Value fields.
func (is *ItemService) applyBuff(player *domain.Player, buffType domain.BuffType, item *domain.Item) UseResult {
	// Add the buff to the player's active buff list
	// Multiple buffs of the same type can coexist (stacking)
	player.AddBuff(buffType, item.Value, item.Duration)

	// Build type-specific message text
	var typeName string
	switch buffType {
	case domain.BuffAttack:
		typeName = "ATK" // Display abbreviation for attack
	case domain.BuffDefense:
		typeName = "DEF" // Display abbreviation for defense
	default:
		typeName = "STAT" // Fallback — shouldn't occur in practice
	}

	return UseResult{
		Success: true,
		Message: fmt.Sprintf("Used %s! %s +%d%% for %d turns.",
			item.Name, typeName, item.Value, item.Duration),
	}
}
