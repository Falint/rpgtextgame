// msg.go defines BubbleTea messages used for screen lifecycle management.
// These messages flow upward from screens to the TUI model.
package screens

import tea "github.com/charmbracelet/bubbletea" // BubbleTea message and command types

// EndScreenMsg is sent by a screen to signal that it has finished.
// The TUI model's Update handler catches this message and sets
// activeScreen to nil, returning to the main menu view.
type EndScreenMsg struct{}

// EndScreenCmd creates a BubbleTea command that produces an EndScreenMsg.
// Called by screen Update() functions when the user presses ESC or
// completes the screen's workflow (e.g., victory in battle).
func EndScreenCmd() tea.Cmd {
	return func() tea.Msg {
		return EndScreenMsg{} // Produce the end signal
	}
}
