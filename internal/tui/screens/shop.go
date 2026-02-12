// Package screens provides game screen implementations for Block 3.
package screens

import (
	"fmt"
	"strings"

	"github.com/charmbracelet/bubbles/key"
	tea "github.com/charmbracelet/bubbletea"
	"github.com/tenyom/textrpg-tui/internal/application"
	"github.com/tenyom/textrpg-tui/internal/domain"
	"github.com/tenyom/textrpg-tui/internal/infrastructure/registry"
	"github.com/tenyom/textrpg-tui/internal/tui/styles"
)

// Pre-allocated key bindings for the shop screen.
var (
	shopKeyUp    = key.NewBinding(key.WithKeys("up", "k"))
	shopKeyDown  = key.NewBinding(key.WithKeys("down", "j"))
	shopKeyEnter = key.NewBinding(key.WithKeys("enter"))
	shopKeyEsc   = key.NewBinding(key.WithKeys("esc", "backspace"))
)

// -----------------------------------------------------------------------------
// Shop Screen (Block 3)
// OUTPUT MATCHES C src/game/shop.c
// -----------------------------------------------------------------------------

// ShopMode represents the current shop submenu.
type ShopMode int

const (
	ShopModeMain ShopMode = iota
	ShopModeBuyWeapons
	ShopModeBuyItems
	ShopModeSell
)

// ShopScreen implements the shop screen.
type ShopScreen struct {
	shopService *application.ShopService
	weapons     *registry.WeaponRegistry
	items       *registry.ItemRegistry
	player      *domain.Player

	mode     ShopMode
	cursor   int
	width    int
	height   int
	message  string // Status message
	msgStyle string // "success", "error", ""
}

// NewShopScreen creates a new shop screen.
func NewShopScreen(
	shopService *application.ShopService,
	weapons *registry.WeaponRegistry,
	items *registry.ItemRegistry,
	player *domain.Player,
) *ShopScreen {
	return &ShopScreen{
		shopService: shopService,
		weapons:     weapons,
		items:       items,
		player:      player,
		mode:        ShopModeMain,
		cursor:      0,
	}
}

// Title returns the screen title.
func (s *ShopScreen) Title() string {
	return "SHOP"
}

// SetSize updates dimensions.
func (s *ShopScreen) SetSize(width, height int) {
	s.width = width
	s.height = height
}

// Update handles shop input.
func (s *ShopScreen) Update(msg tea.Msg) tea.Cmd {
	switch msg := msg.(type) {
	case tea.KeyMsg:
		switch {
		case key.Matches(msg, shopKeyUp):
			if s.cursor > 0 {
				s.cursor--
			}
		case key.Matches(msg, shopKeyDown):
			s.cursor++
		case key.Matches(msg, shopKeyEnter):
			return s.handleSelect()
		case key.Matches(msg, shopKeyEsc):
			if s.mode != ShopModeMain {
				s.mode = ShopModeMain
				s.cursor = 0
				s.message = ""
			} else {
				return EndScreenCmd()
			}
		case msg.String() >= "1" && msg.String() <= "9":
			idx := int(msg.String()[0] - '1')
			s.cursor = idx
			return s.handleSelect()
		}
	}
	return nil
}

// handleSelect processes selection based on mode.
func (s *ShopScreen) handleSelect() tea.Cmd {
	switch s.mode {
	case ShopModeMain:
		switch s.cursor {
		case 0:
			s.mode = ShopModeBuyWeapons
		case 1:
			s.mode = ShopModeBuyItems
		case 2:
			s.mode = ShopModeSell
		case 3:
			return EndScreenCmd()
		}
		s.cursor = 0
		s.message = ""

	case ShopModeBuyWeapons:
		s.handleBuyWeapon()

	case ShopModeBuyItems:
		s.handleBuyItem()

	case ShopModeSell:
		s.handleSell()
	}
	return nil
}

// handleBuyWeapon processes weapon purchase.
func (s *ShopScreen) handleBuyWeapon() {
	entries := s.weapons.GetShopWeapons()
	if s.cursor < 0 || s.cursor >= len(entries) {
		return
	}

	entry := entries[s.cursor]
	if entry.IsSoldOut() {
		s.message = "SOLD OUT!"
		s.msgStyle = "error"
		return
	}

	result := s.shopService.BuyWeapon(entry.Weapon.ID)
	switch result {
	case domain.ResultSuccess:
		s.message = fmt.Sprintf("Purchased %s!", entry.Weapon.Name)
		s.msgStyle = "success"
	case domain.ResultNotEnoughGold:
		s.message = "Not enough gold!"
		s.msgStyle = "error"
	case domain.ResultInventoryFull:
		s.message = "Inventory full!"
		s.msgStyle = "error"
	default:
		s.message = "Purchase failed!"
		s.msgStyle = "error"
	}
}

// handleBuyItem processes item purchase.
func (s *ShopScreen) handleBuyItem() {
	entries := s.items.GetShopItems()
	if s.cursor < 0 || s.cursor >= len(entries) {
		return
	}

	entry := entries[s.cursor]
	if entry.IsSoldOut() {
		s.message = "SOLD OUT!"
		s.msgStyle = "error"
		return
	}

	result := s.shopService.BuyItem(entry.Item.ID, 1)
	switch result {
	case domain.ResultSuccess:
		s.message = fmt.Sprintf("Purchased %s!", entry.Item.Name)
		s.msgStyle = "success"
	case domain.ResultNotEnoughGold:
		s.message = "Not enough gold!"
		s.msgStyle = "error"
	case domain.ResultInventoryFull:
		s.message = "Inventory full!"
		s.msgStyle = "error"
	default:
		s.message = "Purchase failed!"
		s.msgStyle = "error"
	}
}

// handleSell processes item selling.
func (s *ShopScreen) handleSell() {
	// Get sellable items from inventory
	slots := s.player.Inventory.GetOccupiedSlots()
	if s.cursor < 0 || s.cursor >= len(slots) {
		return
	}

	result := s.shopService.SellItem(slots[s.cursor], 1)
	if result == domain.ResultSuccess {
		s.message = "Sold!"
		s.msgStyle = "success"
	} else {
		s.message = "Cannot sell!"
		s.msgStyle = "error"
	}
}

// View renders the shop screen.
// FORMAT MATCHES C src/game/shop.c shop_print_header(), shop_menu()
func (s *ShopScreen) View() string {
	var b strings.Builder

	// Header matching C: shop_print_header()
	b.WriteString(s.renderHeader())
	b.WriteString("\n")

	// Content based on mode
	switch s.mode {
	case ShopModeMain:
		b.WriteString(s.renderMainMenu())
	case ShopModeBuyWeapons:
		b.WriteString(s.renderWeaponList())
	case ShopModeBuyItems:
		b.WriteString(s.renderItemList())
	case ShopModeSell:
		b.WriteString(s.renderSellList())
	}

	// Status message
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

// renderHeader matches C shop_print_header().
func (s *ShopScreen) renderHeader() string {
	var b strings.Builder

	// Title
	title := styles.SectionHeaderStyle.Render("=== SHOP ===")
	b.WriteString(title)
	b.WriteString("\n\n")

	// Gold display
	goldLabel := styles.StatLabelStyle.Render("Your Gold: ")
	goldValue := styles.GoldStyle.Render(fmt.Sprintf("%d G", s.player.Gold))
	b.WriteString("  " + goldLabel + goldValue)

	return b.String()
}

// renderMainMenu matches C shop_menu().
func (s *ShopScreen) renderMainMenu() string {
	var b strings.Builder

	// Menu header
	b.WriteString("\n")
	b.WriteString(styles.SeparatorStyle.Render("--- Shop Menu ---"))
	b.WriteString("\n")

	// Menu items matching C
	items := []string{
		"1. Buy Weapons",
		"2. Buy Items",
		"3. Sell Items",
		"4. Exit Shop",
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

// renderWeaponList matches C shop_print_weapons().
func (s *ShopScreen) renderWeaponList() string {
	var b strings.Builder

	b.WriteString("\n")
	b.WriteString(styles.SectionHeaderStyle.Render("=== WEAPONS ==="))
	b.WriteString("\n\n")

	entries := s.weapons.GetShopWeapons()
	s.clampCursor(len(entries))

	for i, entry := range entries {
		w := entry.Weapon
		displayNum := i + 1

		var line strings.Builder
		line.WriteString(fmt.Sprintf("  %2d. ", displayNum))

		// Sold out check
		if entry.IsSoldOut() {
			line.WriteString(styles.SoldOutStyle.Render("[SOLD OUT] " + w.Name))
		} else {
			// Weapon name with element color
			name := w.Name
			switch w.Element {
			case domain.ElementFire:
				line.WriteString(styles.FireStyle.Render(name))
			case domain.ElementWater:
				line.WriteString(styles.WaterStyle.Render(name))
			case domain.ElementEarth:
				line.WriteString(styles.EarthStyle.Render(name))
			case domain.ElementWind:
				line.WriteString(styles.WindStyle.Render(name))
			default:
				line.WriteString(styles.StatValueStyle.Render(name))
			}

			// Price
			priceStyle := styles.PriceStyle
			if s.player.Gold < w.BuyPrice {
				priceStyle = styles.NotAffordableStyle
			}
			line.WriteString(" - ")
			line.WriteString(priceStyle.Render(fmt.Sprintf("%d G", w.BuyPrice)))

			// Damage
			line.WriteString(styles.StatLabelStyle.Render(" | DMG: "))
			line.WriteString(styles.DamageStyle.Render(fmt.Sprintf("%d", w.BaseDamage)))

			// Element
			if w.Element != domain.ElementNone {
				line.WriteString(fmt.Sprintf(" (%s)", w.Element.String()))
			}

			// Stock
			if !entry.IsUnlimited() {
				line.WriteString(fmt.Sprintf(" [Stock: %d]", entry.Stock))
			}
		}

		if i == s.cursor {
			b.WriteString(styles.SelectedItemStyle.Render(line.String()))
		} else {
			b.WriteString(line.String())
		}
		b.WriteString("\n")
	}

	b.WriteString("\n")
	b.WriteString(styles.HelpDescStyle.Render("[Enter] Buy  [Esc] Back"))

	return b.String()
}

// renderItemList matches C shop_print_items().
func (s *ShopScreen) renderItemList() string {
	var b strings.Builder

	b.WriteString("\n")
	b.WriteString(styles.SectionHeaderStyle.Render("=== ITEMS ==="))
	b.WriteString("\n\n")

	entries := s.items.GetShopItems()
	s.clampCursor(len(entries))

	for i, entry := range entries {
		item := entry.Item
		displayNum := i + 1

		var line strings.Builder
		line.WriteString(fmt.Sprintf("  %2d. ", displayNum))

		if entry.IsSoldOut() {
			line.WriteString(styles.SoldOutStyle.Render("[SOLD OUT] " + item.Name))
		} else {
			line.WriteString(styles.StatValueStyle.Render(item.Name))

			// Price
			priceStyle := styles.PriceStyle
			if s.player.Gold < item.BuyPrice {
				priceStyle = styles.NotAffordableStyle
			}
			line.WriteString(" - ")
			line.WriteString(priceStyle.Render(fmt.Sprintf("%d G", item.BuyPrice)))

			// Description
			line.WriteString(" ")
			line.WriteString(styles.StatLabelStyle.Render("(" + item.Description + ")"))

			// Stock
			if !entry.IsUnlimited() {
				line.WriteString(fmt.Sprintf(" [Stock: %d]", entry.Stock))
			}
		}

		if i == s.cursor {
			b.WriteString(styles.SelectedItemStyle.Render(line.String()))
		} else {
			b.WriteString(line.String())
		}
		b.WriteString("\n")
	}

	b.WriteString("\n")
	b.WriteString(styles.HelpDescStyle.Render("[Enter] Buy  [Esc] Back"))

	return b.String()
}

// renderSellList shows player inventory for selling.
func (s *ShopScreen) renderSellList() string {
	var b strings.Builder

	b.WriteString("\n")
	b.WriteString(styles.SectionHeaderStyle.Render("=== SELL ITEMS ==="))
	b.WriteString("\n\n")

	slots := s.player.Inventory.GetOccupiedSlots()
	s.clampCursor(len(slots))

	if len(slots) == 0 {
		b.WriteString(styles.DisabledItemStyle.Render("  No items to sell"))
		return b.String()
	}

	for i, slotIdx := range slots {
		slot := s.player.Inventory.GetSlot(slotIdx)
		displayNum := i + 1

		var line strings.Builder
		line.WriteString(fmt.Sprintf("  %2d. ", displayNum))

		if slot.Type == domain.SlotWeapon {
			w := slot.Weapon
			line.WriteString(styles.StatValueStyle.Render(w.DisplayName()))
			line.WriteString(" - ")
			sellPrice := s.weapons.GetByID(w.ID).SellPrice / 2
			line.WriteString(styles.PriceStyle.Render(fmt.Sprintf("Sell: %d G", sellPrice)))
		} else if slot.Type == domain.SlotItem {
			item := slot.Item
			line.WriteString(fmt.Sprintf("%s (x%d)", item.Name, slot.Quantity))
			line.WriteString(" - ")
			line.WriteString(styles.PriceStyle.Render(fmt.Sprintf("Sell: %d G", item.SellPrice)))
		}

		if i == s.cursor {
			b.WriteString(styles.SelectedItemStyle.Render(line.String()))
		} else {
			b.WriteString(line.String())
		}
		b.WriteString("\n")
	}

	b.WriteString("\n")
	b.WriteString(styles.HelpDescStyle.Render("[Enter] Sell  [Esc] Back"))

	return b.String()
}

// clampCursor ensures cursor is within bounds.
func (s *ShopScreen) clampCursor(max int) {
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
