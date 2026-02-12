package tui

import (
	"github.com/charmbracelet/lipgloss"
	"github.com/tenyom/textrpg-tui/internal/tui/styles"
)

// View renders the application UI.
func (m AppModel) View() string {
	if m.width == 0 {
		return "Initializing UI..."
	}

	// 1. Render Block 1 (Menu)
	menuView := m.menu.View()

	// 2. Render Block 2 (Character)
	charView := m.character.View()

	// 3. Render Block 3 (Game Output)
	var gameView string
	if m.activeScreen != nil {
		gameView = m.wrapGamePanel(m.activeScreen.View())
	} else {
		gameView = m.wrapGamePanel("\n\n  Welcome to TextRPG!\n\n  Select an option from the menu to begin.")
	}

	// 4. Assemble Panels
	mainContent := lipgloss.JoinHorizontal(
		lipgloss.Top,
		menuView,
		charView,
		gameView,
	)

	// 5. Header and Footer
	header := styles.StatusBarStyle.Width(m.width).Render(" TEXT RPG - PRODUCTION BUILD (Go)")

	help := "[Ctrl+C] Quit  [Arrows] Navigate  [Enter] Select"
	footer := styles.StatusBarStyle.Width(m.width).Render(help)

	return lipgloss.JoinVertical(
		lipgloss.Left,
		header,
		mainContent,
		footer,
	)
}

func (m AppModel) wrapGamePanel(content string) string {
	style := styles.BasePanelStyle
	if m.focusedPanel == PanelGame {
		style = styles.ActivePanelStyle
	}

	// Calculate dimensions (same as resizeComponents)
	mainWidth := m.width - 2
	mainHeight := m.height - HeaderHeight - StatusBarHeight

	menuWidth := (mainWidth * MenuPanelRatio) / 12
	remaining := mainWidth - menuWidth
	charWidth := remaining / 2
	gameWidth := remaining - charWidth

	return style.
		Width(gameWidth).
		Height(mainHeight).
		Render(content)
}
