package screens

import tea "github.com/charmbracelet/bubbletea"

// EndScreenMsg indicates that the current screen has finished.
type EndScreenMsg struct{}

// EndScreenCmd returns a command to signal screen completion.
func EndScreenCmd() tea.Cmd {
	return func() tea.Msg {
		return EndScreenMsg{}
	}
}
