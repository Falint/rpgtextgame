// Package tui implements the terminal user interface using BubbleTea.
// Constants define the 3-panel layout grid system and minimum terminal size.
package tui

// -----------------------------------------------------------------------------
// Panel Constants
// -----------------------------------------------------------------------------

// Panel index constants for the 3-panel layout.
// Used to index into panel width arrays and manage focus state.
const (
	PanelMenu      = iota // Left panel: main menu navigation
	PanelCharacter        // Center panel: character stats and ASCII art
	PanelGame             // Right panel: active game screen (battle, shop, etc.)
	PanelCount            // Total number of panels (used for iteration bounds)
)

// Panel width ratios, using a 12-column grid system (similar to CSS grid).
// The terminal width is divided into 12 equal units, and each panel
// occupies a fraction of the total: 2/12 + 5/12 + 5/12 = 12/12.
const (
	MenuPanelRatio      = 2 // ~17% of terminal width — main menu (narrow)
	CharacterPanelRatio = 5 // ~42% of terminal width — character stats + ASCII art
	GamePanelRatio      = 5 // ~42% of terminal width — active game screen output
)

// -----------------------------------------------------------------------------
// Layout Constants
// -----------------------------------------------------------------------------

// Layout constraints and spacing constants.
const (
	// Minimum terminal dimensions — UI degrades below these sizes
	MinTerminalWidth  = 100 // Minimum columns for readable 3-panel layout
	MinTerminalHeight = 30  // Minimum rows for panel borders, stats, and content

	// Vertical space consumed by header and status bar
	HeaderHeight    = 1 // Top row for screen title
	StatusBarHeight = 1 // Bottom row for status/help text

	// Internal panel spacing
	PanelPadding = 1 // Padding inside each panel border
)
