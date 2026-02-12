// Package screens provides game screen implementations for Block 3.
package screens

import (
	"fmt"
	"strings"

	"github.com/charmbracelet/bubbles/key"
	tea "github.com/charmbracelet/bubbletea"
	"github.com/tenyom/textrpg-tui/internal/application"
	"github.com/tenyom/textrpg-tui/internal/domain"
	"github.com/tenyom/textrpg-tui/internal/tui/styles"
)

// Pre-allocated key bindings for the inventory screen.
var (
	invKeyUp    = key.NewBinding(key.WithKeys("up", "k"))
	invKeyDown  = key.NewBinding(key.WithKeys("down", "j"))
	invKeyEnter = key.NewBinding(key.WithKeys("enter"))
	invKeyEsc   = key.NewBinding(key.WithKeys("esc", "backspace"))
)

// InventoryMode represents the current inventory submenu.
type InventoryMode int

const (
	InventoryModeMain InventoryMode = iota
	InventoryModeWeapons
	InventoryModeConsumables
	InventoryModeMaterials
	InventoryModeUse
)

// InventoryScreen implements the inventory screen.
// It displays the player's items and delegates consumable usage
// to the centralized ItemService for consistent effect handling.
type InventoryScreen struct {
	player      *domain.Player
	itemService *application.ItemService // Centralized item effect processor
	mode        InventoryMode
	cursor      int
	width       int
	height      int
	message     string
	msgStyle    string
}

// NewInventoryScreen creates a new inventory screen.
// itemService must be the same shared instance used by BattleService.
func NewInventoryScreen(player *domain.Player, itemService *application.ItemService) *InventoryScreen {
	return &InventoryScreen{
		player:      player,
		itemService: itemService,
		mode:        InventoryModeMain,
		cursor:      0,
	}
}

// Title returns the screen title.
func (s *InventoryScreen) Title() string {
	return "INVENTORY"
}

// SetSize updates dimensions.
func (s *InventoryScreen) SetSize(width, height int) {
	s.width = width
	s.height = height
}

// Update handles input.
func (s *InventoryScreen) Update(msg tea.Msg) tea.Cmd {
	switch msg := msg.(type) {
	case tea.KeyMsg:
		switch {
		case key.Matches(msg, invKeyUp):
			if s.cursor > 0 {
				s.cursor--
			}
		case key.Matches(msg, invKeyDown):
			s.cursor++
		case key.Matches(msg, invKeyEnter):
			return s.handleSelect()
		case key.Matches(msg, invKeyEsc):
			if s.mode != InventoryModeMain {
				s.mode = InventoryModeMain
				s.cursor = 0
				s.message = ""
			} else {
				return EndScreenCmd()
			}
		case msg.String() >= "1" && msg.String() <= "5":
			idx := int(msg.String()[0] - '1')
			if s.mode == InventoryModeMain {
				s.cursor = idx
				return s.handleSelect()
			}
		}
	}
	return nil
}

// handleSelect processes selection based on mode.
func (s *InventoryScreen) handleSelect() tea.Cmd {
	switch s.mode {
	case InventoryModeMain:
		switch s.cursor {
		case 0:
			s.mode = InventoryModeWeapons
			s.cursor = 0
		case 1:
			s.mode = InventoryModeConsumables
			s.cursor = 0
		case 2:
			s.mode = InventoryModeMaterials
			s.cursor = 0
		case 3:
			s.mode = InventoryModeUse
			s.cursor = 0
		case 4: // Back
			return EndScreenCmd()
		}

	case InventoryModeUse:
		s.handleUseItem()

	// View modes do nothing on enter (just Esc to back)
	case InventoryModeWeapons, InventoryModeConsumables, InventoryModeMaterials:
		return nil
	}
	return nil
}

// handleUseItem uses the selected consumable via the centralized ItemService.
// This ensures consistent behavior with battle item usage (same validation,
// same effect logic, same HP-full check).
func (s *InventoryScreen) handleUseItem() {
	consumables := s.player.Inventory.GetConsumables()
	if len(consumables) == 0 {
		s.message = "No consumables available."
		s.msgStyle = "error"
		return
	}
	s.clampCursor(len(consumables))

	slot := consumables[s.cursor]
	item := slot.Item

	// Delegate to ItemService — single source of truth for item effects
	result := s.itemService.UseConsumable(item)

	if !result.Success {
		// Effect was rejected (e.g., HP full). Show error, do NOT remove item.
		s.message = result.Error
		s.msgStyle = "error"
		return
	}

	// Effect applied — remove one unit from inventory.
	// Find the actual slot index for this consumable to remove correctly.
	slotIndex := s.findConsumableSlotIndex(s.cursor)
	if slotIndex != -1 {
		s.player.Inventory.RemoveItem(slotIndex, 1)
	}

	// Display success message
	s.message = result.Message
	s.msgStyle = "success"
}

// findConsumableSlotIndex returns the inventory slot index of the Nth consumable.
// This is necessary because consumables are a filtered view of the full inventory.
func (s *InventoryScreen) findConsumableSlotIndex(n int) int {
	count := 0
	for i := 0; i < domain.MaxInventorySlots; i++ {
		slot := s.player.Inventory.GetSlot(i)
		if slot != nil && slot.Type == domain.SlotItem && slot.Item.Category == domain.ItemConsumable {
			if count == n {
				return i
			}
			count++
		}
	}
	return -1
}

// View renders the screen.
func (s *InventoryScreen) View() string {
	var b strings.Builder

	// Header matching C
	b.WriteString(styles.SectionHeaderStyle.Render("=== INVENTORY ==="))
	b.WriteString("\n\n")

	switch s.mode {
	case InventoryModeMain:
		b.WriteString(s.renderMainMenu())
	case InventoryModeWeapons:
		b.WriteString(s.renderWeapons())
	case InventoryModeConsumables:
		b.WriteString(s.renderConsumables())
	case InventoryModeMaterials:
		b.WriteString(s.renderMaterials())
	case InventoryModeUse:
		b.WriteString(s.renderUseItem())
	}

	// Message
	if s.message != "" {
		b.WriteString("\n")
		switch s.msgStyle {
		case "success":
			b.WriteString(styles.SuccessStyle.Render(s.message))
		case "error":
			b.WriteString(styles.ErrorStyle.Render(s.message))
		default:
			b.WriteString(s.message)
		}
	}

	return b.String()
}

func (s *InventoryScreen) renderMainMenu() string {
	var b strings.Builder

	// All items summary count
	total := s.player.Inventory.UsedSlots()
	b.WriteString(fmt.Sprintf("Total Items: %d/%d\n\n", total, domain.MaxInventorySlots))

	items := []string{
		"1. View Weapons",
		"2. View Consumables",
		"3. View Materials",
		"4. Use Item",
		"5. Back",
	}

	for i, item := range items {
		if i == s.cursor {
			b.WriteString(styles.SelectedItemStyle.Render(item))
		} else {
			b.WriteString(styles.NormalItemStyle.Render(item))
		}
		b.WriteString("\n")
	}

	b.WriteString("Choice: ")
	return b.String()
}

func (s *InventoryScreen) renderWeapons() string {
	var b strings.Builder
	b.WriteString(styles.SectionHeaderStyle.Render("--- WEAPONS ---"))
	b.WriteString("\n")

	weapons := s.player.Inventory.GetWeapons()
	if len(weapons) == 0 {
		b.WriteString(styles.DisabledItemStyle.Render("  No weapons"))
	} else {
		for i, w := range weapons {
			b.WriteString(fmt.Sprintf("%2d. %s\n", i+1, w.DisplayName()))
		}
	}

	b.WriteString("\n")
	b.WriteString(styles.HelpDescStyle.Render("[Esc] Back"))
	return b.String()
}

func (s *InventoryScreen) renderConsumables() string {
	var b strings.Builder
	b.WriteString(styles.SectionHeaderStyle.Render("--- CONSUMABLES ---"))
	b.WriteString("\n")

	consumables := s.player.Inventory.GetConsumables()
	if len(consumables) == 0 {
		b.WriteString(styles.DisabledItemStyle.Render("  No consumables"))
	} else {
		for i, slot := range consumables {
			b.WriteString(fmt.Sprintf("%2d. %s x%d\n", i+1, slot.Item.Name, slot.Quantity))
		}
	}

	b.WriteString("\n")
	b.WriteString(styles.HelpDescStyle.Render("[Esc] Back"))
	return b.String()
}

func (s *InventoryScreen) renderMaterials() string {
	var b strings.Builder
	b.WriteString(styles.SectionHeaderStyle.Render("--- MATERIALS ---"))
	b.WriteString("\n")

	materials := s.player.Inventory.GetMaterials()
	if len(materials) == 0 {
		b.WriteString(styles.DisabledItemStyle.Render("  No materials"))
	} else {
		for i, mat := range materials {
			b.WriteString(fmt.Sprintf("%2d. %s x%d\n", i+1, mat.Name, mat.Quantity))
		}
	}

	b.WriteString("\n")
	b.WriteString(styles.HelpDescStyle.Render("[Esc] Back"))
	return b.String()
}

func (s *InventoryScreen) renderUseItem() string {
	var b strings.Builder
	b.WriteString(styles.SectionHeaderStyle.Render("--- USE ITEM ---"))
	b.WriteString("\n")

	consumables := s.player.Inventory.GetConsumables()
	if len(consumables) == 0 {
		b.WriteString(styles.DisabledItemStyle.Render("  No items to use"))
		b.WriteString("\n")
		b.WriteString(styles.HelpDescStyle.Render("[Esc] Back"))
		return b.String()
	}

	s.clampCursor(len(consumables))
	for i, slot := range consumables {
		var line string
		line = fmt.Sprintf("%2d. %s x%d", i+1, slot.Item.Name, slot.Quantity)

		if i == s.cursor {
			b.WriteString(styles.SelectedItemStyle.Render(line))
		} else {
			b.WriteString(styles.NormalItemStyle.Render(line))
		}
		b.WriteString("\n")
	}

	b.WriteString("\n")
	b.WriteString(styles.HelpDescStyle.Render("[Enter] Use  [Esc] Back"))
	return b.String()
}

// clampCursor ensures cursor is within bounds.
func (s *InventoryScreen) clampCursor(max int) {
	if max <= 0 {
		s.cursor = 0
		return
	}
	if s.cursor >= max {
		s.cursor = max - 1
	}
	if s.cursor < 0 {
		s.cursor = 0
	}
}
