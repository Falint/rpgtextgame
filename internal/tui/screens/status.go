// Package screens provides game screen implementations for Block 3.
package screens

import (
	"fmt"
	"strings"

	tea "github.com/charmbracelet/bubbletea"
	"github.com/tenyom/textrpg-tui/internal/domain"
	"github.com/tenyom/textrpg-tui/internal/tui/styles"
)

// -----------------------------------------------------------------------------
// Status Screen (Block 3)
// OUTPUT MATCHES C src/core/player.c player_print_detailed()
// -----------------------------------------------------------------------------

// StatusScreen implements the player status screen.
type StatusScreen struct {
	player *domain.Player
}

// NewStatusScreen creates a new status screen.
func NewStatusScreen(player *domain.Player) *StatusScreen {
	return &StatusScreen{
		player: player,
	}
}

// Title returns the screen title.
func (s *StatusScreen) Title() string {
	return "STATUS"
}

// SetSize updates dimensions.
func (s *StatusScreen) SetSize(width, height int) {
	// No resize needed for static view
}

// Update handles input.
func (s *StatusScreen) Update(msg tea.Msg) tea.Cmd {
	switch msg := msg.(type) {
	case tea.KeyMsg:
		switch msg.String() {
		case "esc", "backspace":
			return EndScreenCmd()
		}
	}
	return nil
}

// View renders the screen.
// FORMAT MATCHES C player_print_detailed()
func (s *StatusScreen) View() string {
	var b strings.Builder

	// Header matching C
	b.WriteString(styles.SectionHeaderStyle.Render("=== PLAYER STATUS ==="))
	b.WriteString("\n\n")

	// Name & Level (C doesn't show level but we can match basic info)
	b.WriteString(fmt.Sprintf("Name: %s\n", s.player.Name))
	b.WriteString(fmt.Sprintf("HP: %d/%d\n", s.player.CurrentHP, s.player.MaxHP))
	b.WriteString(fmt.Sprintf("Gold: %d\n", s.player.Gold))
	b.WriteString("\n")

	// Combat Stats
	b.WriteString(styles.SectionHeaderStyle.Render("--- Combat Stats ---"))
	b.WriteString("\n")
	b.WriteString(fmt.Sprintf("Base Attack:  %d\n", s.player.BaseAttack))
	b.WriteString(fmt.Sprintf("Base Defense: %d\n", s.player.BaseDef))

	totalAtk := s.player.GetAttack()
	totalDef := s.player.GetDefense()

	b.WriteString(fmt.Sprintf("Total Attack: %d", totalAtk))
	if totalAtk > s.player.BaseAttack {
		b.WriteString(styles.SuccessStyle.Render(fmt.Sprintf(" (+%d)", totalAtk-s.player.BaseAttack)))
	}
	b.WriteString("\n")

	b.WriteString(fmt.Sprintf("Total Defense: %d", totalDef))
	if totalDef > s.player.BaseDef {
		b.WriteString(styles.SuccessStyle.Render(fmt.Sprintf(" (+%d)", totalDef-s.player.BaseDef)))
	}
	b.WriteString("\n\n")

	// Equipment
	b.WriteString(styles.SectionHeaderStyle.Render("--- Equipment ---"))
	b.WriteString("\n")
	b.WriteString("Weapon: ")
	if s.player.EquippedWeapon != nil {
		w := s.player.EquippedWeapon
		b.WriteString(w.DisplayName())
		b.WriteString(fmt.Sprintf("\n  Type: %s", w.Type.String()))
		b.WriteString(fmt.Sprintf("\n  Damage: %d", w.Damage()))
		if w.Element != domain.ElementNone {
			b.WriteString(fmt.Sprintf("\n  Element: %s", w.Element.String()))
		}
	} else {
		b.WriteString("None")
	}
	b.WriteString("\n\n")

	// Active Buffs
	b.WriteString(styles.SectionHeaderStyle.Render("--- Active Buffs ---"))
	b.WriteString("\n")
	if len(s.player.Buffs) == 0 {
		b.WriteString("None")
	} else {
		for _, buff := range s.player.Buffs {
			name := "Unknown"
			switch buff.Type {
			case domain.BuffAttack:
				name = "Attack Up"
			case domain.BuffDefense:
				name = "Defense Up"
			}
			b.WriteString(fmt.Sprintf("%s (%d turns)", name, buff.TurnsRemaining))
			b.WriteString("\n")
		}
	}

	b.WriteString("\n\n")
	b.WriteString(styles.HelpDescStyle.Render("[Esc] Back"))

	return b.String()
}
