// Package screens provides game screen implementations for Block 3 (game output panel).
// Each screen replicates the exact C output format for its corresponding game function.
//
// DESIGN: All screens implement the Screen interface, allowing the TUI model
// to polymorphically manage any screen in the game panel without type assertions.
// Screen lifecycle: created via factory → receives input via Update → renders via View → signals end via EndScreenCmd.
package screens

import tea "github.com/charmbracelet/bubbletea"

// Screen is the contract for all game screens in Block 3.
// Each screen is a self-contained mini-application within the game panel.
// The TUI model delegates all input and rendering to the active screen.
type Screen interface {
	// Update processes key/mouse input and returns BubbleTea commands.
	// EndScreenCmd() should be returned to signal screen completion.
	Update(msg tea.Msg) tea.Cmd

	// View renders the screen content as a string for the game panel.
	View() string

	// Title returns the screen title shown in the panel header.
	Title() string

	// SetSize updates the available width/height when terminal is resized.
	SetSize(width, height int)
}

// ScreenType identifies which screen is currently active.
// Used for conditional logic in the Update function.
type ScreenType int

// Screen type constants for each game screen.
const (
	ScreenMainMenu  ScreenType = iota // No active screen — showing welcome message
	ScreenBattle                      // Turn-based combat screen
	ScreenShop                        // Buy/sell items and weapons
	ScreenUpgrade                     // Weapon tier upgrade screen
	ScreenEquip                       // Weapon equip/unequip screen
	ScreenInventory                   // View/use items in inventory
	ScreenStatus                      // Player stats overview
)
