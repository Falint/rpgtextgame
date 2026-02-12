// Package application provides game services and orchestration.
//
// item_service.go — Centralized Item Effect Processor
//
// RESPONSIBILITY:
//
//	This service is the single source of truth for applying consumable item
//	effects to a player. Both BattleService and InventoryScreen delegate
//	item effect logic here, ensuring consistent behavior across all contexts.
//
// DESIGN RATIONALE:
//
//	Previously, item effects were duplicated: BattleService used player.Heal()
//	while InventoryScreen manually mutated player.CurrentHP. Buff logic was
//	entirely missing from inventory. This service eliminates that duplication
//	and enforces validation (e.g., HP-full check) in one place.
//
// DEPENDENCIES:
//   - domain.Player (target of all effects)
//   - domain.Item / domain.ConsumableType (defines what effect to apply)
//
// DOES NOT:
//   - Remove items from inventory (callers handle this — different contexts
//     use different removal strategies: slot-index in battle, item-ID in inventory)
//   - Manage battle turns or UI state
package application

import (
	"fmt"

	"github.com/tenyom/textrpg-tui/internal/domain"
)

// -----------------------------------------------------------------------------
// Item Service — Centralized Consumable Effect Processor
// -----------------------------------------------------------------------------

// UseResult carries the outcome of attempting to use a consumable item.
// It separates success/failure state from display messages, allowing callers
// (battle UI, inventory UI, future quick-use) to handle presentation independently.
type UseResult struct {
	// Success indicates whether the item effect was applied.
	// When false, the item should NOT be removed from inventory.
	Success bool

	// Message is a human-readable description of the outcome.
	// Examples: "Recovered 30 HP!", "ATK +25% for 3 turns."
	Message string

	// Error is set only when Success is false.
	// Examples: "HP is already full!", "Cannot use this item."
	Error string
}

// ItemService processes consumable item effects on a player.
// It applies validation and effects but does NOT manage inventory removal —
// that responsibility belongs to the caller's context.
type ItemService struct {
	player *domain.Player
}

// NewItemService creates an ItemService bound to the given player.
func NewItemService(player *domain.Player) *ItemService {
	return &ItemService{
		player: player,
	}
}

// UseConsumable validates and applies a consumable item's effect to the player.
//
// Returns a UseResult indicating success/failure and a descriptive message.
// The caller is responsible for removing the item from inventory only if
// result.Success is true.
//
// Supported consumable types:
//   - ConsumeHeal: Restores HP by item.Value. Fails if HP is already full.
//   - ConsumeFullRestore: Restores HP to maximum. Fails if HP is already full.
//   - ConsumeBuffAtk: Adds an attack buff for item.Duration turns.
//   - ConsumeBuffDef: Adds a defense buff for item.Duration turns.
func (s *ItemService) UseConsumable(item *domain.Item) UseResult {
	// Guard: only consumables can be used
	if item.Category != domain.ItemConsumable {
		return UseResult{
			Success: false,
			Error:   "This item cannot be used.",
		}
	}

	switch item.ConsumableType {
	case domain.ConsumeHeal:
		return s.applyHeal(item)

	case domain.ConsumeFullRestore:
		return s.applyFullRestore(item)

	case domain.ConsumeBuffAtk:
		return s.applyBuff(item, domain.BuffAttack, "ATK")

	case domain.ConsumeBuffDef:
		return s.applyBuff(item, domain.BuffDefense, "DEF")

	default:
		return UseResult{
			Success: false,
			Error:   fmt.Sprintf("Unknown consumable type for %s.", item.Name),
		}
	}
}

// applyHeal restores HP by item.Value, capped at MaxHP.
// Fails with a descriptive error if player HP is already at maximum.
func (s *ItemService) applyHeal(item *domain.Item) UseResult {
	// Validate: prevent wasting a potion when HP is already full
	if s.player.CurrentHP >= s.player.MaxHP {
		return UseResult{
			Success: false,
			Error:   "HP is already full!",
		}
	}

	// Record HP before healing to calculate actual amount recovered
	oldHP := s.player.CurrentHP

	// Delegate to domain method which handles MaxHP capping
	s.player.Heal(item.Value)

	recovered := s.player.CurrentHP - oldHP

	return UseResult{
		Success: true,
		Message: fmt.Sprintf("Used %s! Recovered %d HP.", item.Name, recovered),
	}
}

// applyFullRestore sets HP to MaxHP.
// Fails if player HP is already at maximum.
func (s *ItemService) applyFullRestore(item *domain.Item) UseResult {
	if s.player.CurrentHP >= s.player.MaxHP {
		return UseResult{
			Success: false,
			Error:   "HP is already full!",
		}
	}

	s.player.CurrentHP = s.player.MaxHP

	return UseResult{
		Success: true,
		Message: fmt.Sprintf("Used %s! Fully restored!", item.Name),
	}
}

// applyBuff adds a temporary stat buff to the player.
// buffLabel is used for the message (e.g., "ATK", "DEF").
func (s *ItemService) applyBuff(item *domain.Item, buffType domain.BuffType, buffLabel string) UseResult {
	s.player.AddBuff(buffType, item.Value, item.Duration)

	return UseResult{
		Success: true,
		Message: fmt.Sprintf("Used %s! %s +%d%% for %d turns.", item.Name, buffLabel, item.Value, item.Duration),
	}
}
