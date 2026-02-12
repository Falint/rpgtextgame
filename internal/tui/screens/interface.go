// Package screens provides game screen implementations for Block 3.
// Each screen replicates the exact C output for its function.
package screens

import tea "github.com/charmbracelet/bubbletea"

// Screen is the interface for all game screens in Block 3.
// Each screen must implement View() for rendering and Update() for input.
type Screen interface {
	// Update handles input and returns commands.
	Update(msg tea.Msg) tea.Cmd

	// View renders the screen content.
	View() string

	// Title returns the screen title for the header.
	Title() string

	// SetSize updates screen dimensions.
	SetSize(width, height int)
}

// ScreenType identifies which screen is active.
type ScreenType int

const (
	ScreenMainMenu ScreenType = iota
	ScreenBattle
	ScreenShop
	ScreenUpgrade
	ScreenEquip
	ScreenInventory
	ScreenStatus
)
