// Package screens provides game screen implementations for Block 3.
package screens

import (
	"fmt"
	"strings"

	"github.com/charmbracelet/bubbles/key"
	tea "github.com/charmbracelet/bubbletea"
	"github.com/tenyom/textrpg-tui/internal/domain"
	"github.com/tenyom/textrpg-tui/internal/tui/styles"
)

// Pre-allocated key bindings for the equip screen.
var (
	equipKeyUp    = key.NewBinding(key.WithKeys("up", "k"))
	equipKeyDown  = key.NewBinding(key.WithKeys("down", "j"))
	equipKeyEnter = key.NewBinding(key.WithKeys("enter"))
	equipKeyEsc   = key.NewBinding(key.WithKeys("esc", "backspace"))
)

// EquipScreen implements the weapon equip screen.
type EquipScreen struct {
	player   *domain.Player
	cursor   int
	width    int
	height   int
	message  string
	msgStyle string
}

// NewEquipScreen creates a new equip screen.
func NewEquipScreen(player *domain.Player) *EquipScreen {
	return &EquipScreen{
		player: player,
		cursor: 0,
	}
}

// Title returns the screen title.
func (s *EquipScreen) Title() string {
	return "EQUIP WEAPON"
}

// SetSize updates dimensions.
func (s *EquipScreen) SetSize(width, height int) {
	s.width = width
	s.height = height
}

// Update handles input.
func (s *EquipScreen) Update(msg tea.Msg) tea.Cmd {
	switch msg := msg.(type) {
	case tea.KeyMsg:
		switch {
		case key.Matches(msg, equipKeyUp):
			if s.cursor > 0 {
				s.cursor--
			}
		case key.Matches(msg, equipKeyDown):
			s.cursor++
		case key.Matches(msg, equipKeyEnter):
			s.handleEquip()
		case key.Matches(msg, equipKeyEsc):
			return EndScreenCmd()
		}
	}
	return nil
}

// handleEquip equips the selected weapon.
func (s *EquipScreen) handleEquip() {
	weapons := s.getEquippableWeapons()
	if len(weapons) == 0 {
		return
	}

	s.clampCursor(len(weapons))

	slotIdx := weapons[s.cursor].SlotIdx

	// Unequip current weapon if any
	oldWeapon := s.player.EquippedWeapon
	if oldWeapon != nil {
		s.player.Inventory.AddWeapon(oldWeapon)
	}

	// Equip new weapon
	weapon, result := s.player.Inventory.RemoveWeapon(slotIdx)
	if result == domain.ResultSuccess {
		s.player.EquippedWeapon = weapon
		s.message = fmt.Sprintf("Equipped %s!", weapon.Name)
		s.msgStyle = "success"
	} else {
		s.message = "Failed to equip!"
		s.msgStyle = "error"
	}
}

type equippableEntry struct {
	Weapon  *domain.Weapon
	SlotIdx int
}

func (s *EquipScreen) getEquippableWeapons() []equippableEntry {
	var list []equippableEntry
	for i := 0; i < domain.MaxInventorySlots; i++ {
		slot := s.player.Inventory.GetSlot(i)
		if slot != nil && slot.Type == domain.SlotWeapon {
			list = append(list, equippableEntry{
				Weapon:  slot.Weapon,
				SlotIdx: i,
			})
		}
	}
	return list
}

// View renders the screen.
func (s *EquipScreen) View() string {
	var b strings.Builder

	// Header matching C
	b.WriteString(styles.SectionHeaderStyle.Render("=== EQUIP WEAPON ==="))
	b.WriteString("\n\n")

	// Currently Equipped
	b.WriteString("Currently Equipped: ")
	if s.player.EquippedWeapon != nil {
		w := s.player.EquippedWeapon
		b.WriteString(styles.StatValueStyle.Render(w.DisplayName()))
		b.WriteString(fmt.Sprintf(" (DMG: %d)", w.Damage()))
	} else {
		b.WriteString(styles.DisabledItemStyle.Render("None"))
	}
	b.WriteString("\n\n")

	// Inventory Weapons
	b.WriteString(styles.SectionHeaderStyle.Render("--- INVENTORY WEAPONS ---"))
	b.WriteString("\n")

	weapons := s.getEquippableWeapons()

	if len(weapons) == 0 {
		b.WriteString(styles.DisabledItemStyle.Render("  No weapons in inventory"))
	} else {
		s.clampCursor(len(weapons))
		for i, entry := range weapons {
			w := entry.Weapon
			line := fmt.Sprintf("%2d. %s (DMG: %d)", i+1, w.DisplayName(), w.Damage())

			if i == s.cursor {
				b.WriteString(styles.SelectedItemStyle.Render(line))
			} else {
				b.WriteString(styles.NormalItemStyle.Render(line))
			}
			b.WriteString("\n")
		}
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

	b.WriteString("\n")
	b.WriteString(styles.HelpDescStyle.Render("[Enter] Equip  [Esc] Back"))

	return b.String()
}

// clampCursor ensures cursor is within bounds.
func (s *EquipScreen) clampCursor(max int) {
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
