// Package tui provides the BubbleTea-based terminal UI.
package tui

// -----------------------------------------------------------------------------
// Panel Constants
// -----------------------------------------------------------------------------

const (
	// Panel indices
	PanelMenu = iota
	PanelCharacter
	PanelGame
	PanelCount
)

// Panel width ratios (out of 12, like CSS grid)
const (
	MenuPanelRatio      = 2 // ~17% - Main menu
	CharacterPanelRatio = 5 // ~42% - Character + ASCII
	GamePanelRatio      = 5 // ~42% - Game output
)

// -----------------------------------------------------------------------------
// Layout Constants
// -----------------------------------------------------------------------------

const (
	// Minimum terminal dimensions
	MinTerminalWidth  = 100
	MinTerminalHeight = 30

	// Header and footer heights
	HeaderHeight    = 1
	StatusBarHeight = 1

	// Panel padding
	PanelPadding = 1
)
