// update.go implements the Update function of the Elm Architecture.
// This is the central message dispatcher for the TUI application.
// It processes keyboard input, window resize events, and screen
// lifecycle messages, coordinating between the menu, character panel,
// and the active game screen.
package tui

import (
	tea "github.com/charmbracelet/bubbletea"                // BubbleTea framework types
	"github.com/tenyom/textrpg-tui/internal/domain"         // MonsterType for battle selection
	"github.com/tenyom/textrpg-tui/internal/tui/components" // Menu component and menu item IDs
	"github.com/tenyom/textrpg-tui/internal/tui/screens"    // Screen factory functions and messages
)

// Update handles all incoming BubbleTea messages.
// Message processing order:
//  1. Global keys (Ctrl+C to quit)
//  2. Window resize events
//  3. Screen lifecycle messages (EndScreenMsg)
//  4. Active screen input forwarding
//  5. Menu navigation and selection
func (m AppModel) Update(msg tea.Msg) (tea.Model, tea.Cmd) {
	var cmd tea.Cmd
	var cmds []tea.Cmd

	switch msg := msg.(type) {
	case tea.KeyMsg:
		switch msg.String() {
		case "ctrl+c":
			return m, tea.Quit
		}

	case tea.WindowSizeMsg:
		m.width = msg.Width
		m.height = msg.Height
		m.resizeComponents()
		// Redraw
		return m, nil

	case screens.EndScreenMsg:
		m.activeScreen = nil
		m.focusedPanel = PanelMenu
		m.menu.SetFocused(true)
		m.character.SetFocused(false)
		return m, nil
	}

	// Route input based on focus
	if m.focusedPanel == PanelMenu {
		// Update Menu
		cmd = m.menu.Update(msg) // Note: This mutates m.menu
		cmds = append(cmds, cmd)

		// Check for selection
		if keyMsg, ok := msg.(tea.KeyMsg); ok {
			trigger := false
			if keyMsg.String() == "enter" {
				trigger = true
			} else if len(keyMsg.String()) == 1 && keyMsg.String() >= "1" && keyMsg.String() <= "9" {
				trigger = true
			}

			if trigger {
				// Menu item selected
				item := m.menu.SelectedItem()
				cmd = m.handleMenuSelect(item.ID)
				cmds = append(cmds, cmd)
			}
		}

	} else if m.focusedPanel == PanelGame && m.activeScreen != nil {
		// Update Game Screen
		cmd = m.activeScreen.Update(msg)
		cmds = append(cmds, cmd)
	}

	return m, tea.Batch(cmds...)
}

func (m *AppModel) handleMenuSelect(id string) tea.Cmd {
	switch id {
	case components.MenuBattleNormal:
		if m.battleService.StartBattle(domain.MonsterNormal) {
			m.activeScreen = screens.NewBattleScreen(m.battleService, m.player)
			m.focusGame()
		}
	case components.MenuBattleElite:
		if m.battleService.StartBattle(domain.MonsterElite) {
			m.activeScreen = screens.NewBattleScreen(m.battleService, m.player)
			m.focusGame()
		}
	case components.MenuBattleBoss:
		if m.battleService.StartBattle(domain.MonsterBoss) {
			m.activeScreen = screens.NewBattleScreen(m.battleService, m.player)
			m.focusGame()
		}
	case components.MenuShop:
		m.activeScreen = screens.NewShopScreen(m.shopService, m.weapons, m.items, m.player)
		m.focusGame()
	case components.MenuUpgrade:
		m.activeScreen = screens.NewUpgradeScreen(m.player, m.weapons, m.items, m.upgrades)
		m.focusGame()
	case components.MenuEquip:
		m.activeScreen = screens.NewEquipScreen(m.player)
		m.focusGame()
	case components.MenuInventory:
		m.activeScreen = screens.NewInventoryScreen(m.player, m.itemService)
		m.focusGame()
	case components.MenuStatus:
		m.activeScreen = screens.NewStatusScreen(m.player)
		m.focusGame()
	case components.MenuExit:
		return tea.Quit
	}

	// Refresh components if size is set
	if m.width > 0 && m.height > 0 && m.activeScreen != nil {
		m.resizeComponents()
	}

	return nil
}

func (m *AppModel) focusGame() {
	m.focusedPanel = PanelGame
	m.menu.SetFocused(false)
	// Character panel never receives input focus
}

func (m *AppModel) resizeComponents() {
	if m.width == 0 || m.height == 0 {
		return
	}

	mainWidth := m.width - 2 // Global margin
	mainHeight := m.height - HeaderHeight - StatusBarHeight - 2

	// Calculate widths based on ratio 2:5:5
	menuWidth := (mainWidth * MenuPanelRatio) / 12
	// Distribute remaining
	remaining := mainWidth - menuWidth
	charWidth := remaining / 2
	gameWidth := remaining - charWidth // Handle odd numbers

	// Height is shared
	panelHeight := mainHeight

	// Resize components
	m.menu.SetSize(menuWidth, panelHeight)
	m.character.SetSize(charWidth, panelHeight)

	if m.activeScreen != nil {
		// activeScreen SetSize doesn't return anything, assuming interface has SetSize
		// Wait, interface needs SetSize method!
		if s, ok := m.activeScreen.(interface{ SetSize(int, int) }); ok {
			s.SetSize(gameWidth, panelHeight)
		}
	}
}
