package components

import (
	"fmt"
	"strings"

	"github.com/charmbracelet/bubbles/key"
	tea "github.com/charmbracelet/bubbletea"
	"github.com/tenyom/textrpg-tui/internal/tui/styles"
)

// -----------------------------------------------------------------------------
// Key Bindings
// -----------------------------------------------------------------------------

var (
	keyUp = key.NewBinding(
		key.WithKeys("up", "k"),
	)
	keyDown = key.NewBinding(
		key.WithKeys("down", "j"),
	)
	keyEnter = key.NewBinding(
		key.WithKeys("enter"),
	)
)

// -----------------------------------------------------------------------------
// Menu Component (Block 1)
// -----------------------------------------------------------------------------

// Menu is the main menu panel (left side).
// EXACT OUTPUT PARITY with C src/main.c print_main_menu()
type Menu struct {
	items   []MenuItem
	cursor  int
	width   int
	height  int
	focused bool
}

// NewMenu creates a new main menu with all 9 items from C.
func NewMenu() Menu {
	return Menu{
		items:   MainMenuItems(),
		cursor:  0,
		focused: true, // Start focused
	}
}

// SetSize updates component dimensions.
func (m *Menu) SetSize(width, height int) {
	m.width = width
	m.height = height
}

// SetFocused sets focus state.
func (m *Menu) SetFocused(focused bool) {
	m.focused = focused
}

// SelectedItem returns currently selected menu item.
func (m *Menu) SelectedItem() MenuItem {
	if m.cursor >= 0 && m.cursor < len(m.items) {
		return m.items[m.cursor]
	}
	return MenuItem{}
}

// Cursor returns current cursor position.
func (m *Menu) Cursor() int {
	return m.cursor
}

// Update handles input for menu navigation.
func (m *Menu) Update(msg tea.Msg) tea.Cmd {
	switch msg := msg.(type) {
	case tea.KeyMsg:
		switch {
		case key.Matches(msg, keyUp):
			if m.cursor > 0 {
				m.cursor--
			}
		case key.Matches(msg, keyDown):
			if m.cursor < len(m.items)-1 {
				m.cursor++
			}
		// Number keys 1-9 for direct selection
		case msg.String() >= "1" && msg.String() <= "9":
			idx := int(msg.String()[0] - '1')
			if idx >= 0 && idx < len(m.items) {
				m.cursor = idx
			}
		}
	}
	return nil
}

// View renders the menu.
// OUTPUT MATCHES C src/main.c print_main_menu() format:
//
//	=== MAIN MENU ===
//	1. Battle (Normal)
//	2. Battle (Elite)
//	...
//	=================
//	Choice:
func (m Menu) View() string {
	var b strings.Builder

	// Title matching C: "=== MAIN MENU ==="
	title := styles.SectionHeaderStyle.Render("=== MAIN MENU ===")
	b.WriteString(title)
	b.WriteString("\n")

	// Menu items (1-9)
	for i, item := range m.items {
		prefix := fmt.Sprintf("%s. ", item.Key)

		var line string
		if i == m.cursor && m.focused {
			line = styles.SelectedItemStyle.Render(prefix + item.Label)
		} else {
			line = styles.NormalItemStyle.Render(prefix + item.Label)
		}

		b.WriteString(line)
		b.WriteString("\n")
	}

	// Separator matching C: "================="
	sep := styles.SeparatorStyle.Render("=================")
	b.WriteString(sep)
	b.WriteString("\n")

	// Choice prompt
	b.WriteString("Choice: ")

	return m.applyPanelStyle(b.String())
}

// applyPanelStyle applies the appropriate panel style.
func (m Menu) applyPanelStyle(content string) string {
	style := styles.BasePanelStyle
	if m.focused {
		style = styles.ActivePanelStyle
	}

	return style.
		Width(m.width).
		Height(m.height).
		Render(content)
}
