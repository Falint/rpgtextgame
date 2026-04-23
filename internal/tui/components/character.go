// Package components provides reusable TUI components.
package components

import (
	"fmt"     // Sprintf for stat value formatting
	"strings" // Builder for efficient string concatenation, Repeat for HP bar

	"github.com/tenyom/textrpg-tui/internal/domain"     // Player entity and weapon/buff types
	"github.com/tenyom/textrpg-tui/internal/tui/styles" // Centralized style definitions
)

// HP display thresholds and bar sizing constants.
// Named constants prevent magic numbers in HP rendering logic.
const (
	HPLowThreshold = 0.3 // HP percentage below which text turns red (30%)
	HPBarMinWidth  = 10  // Minimum character width for the HP bar
	HPBarMaxWidth  = 30  // Maximum character width to prevent oversized bars
	HPBarPadding   = 8   // Horizontal padding subtracted from panel width for bar sizing
)

// -----------------------------------------------------------------------------
// Character Component (Block 2)
// -----------------------------------------------------------------------------

// Character displays player status with ASCII art.
// OUTPUT MATCHES C src/core/player.c player_print_status()
type Character struct {
	player  *domain.Player
	width   int
	height  int
	focused bool
}

// NewCharacter creates a new character status panel.
func NewCharacter(player *domain.Player) Character {
	return Character{
		player:  player,
		focused: false,
	}
}

// SetPlayer updates the player reference.
func (c *Character) SetPlayer(player *domain.Player) {
	c.player = player
}

// SetSize updates component dimensions.
func (c *Character) SetSize(width, height int) {
	c.width = width
	c.height = height
}

// SetFocused sets focus state.
func (c *Character) SetFocused(focused bool) {
	c.focused = focused
}

// View renders the character status panel.
// FORMAT MATCHES C player_print_status() and player_print_detailed()
func (c Character) View() string {
	if c.player == nil {
		return c.applyPanelStyle("No player data")
	}

	var b strings.Builder

	// Title
	title := styles.PanelTitleStyle.Render("⚔️ CHARACTER STATUS")
	b.WriteString(title)
	b.WriteString("\n\n")

	// Basic stats (matching C format)
	b.WriteString(c.renderBasicStats())
	b.WriteString("\n")

	// HP Bar
	b.WriteString(c.renderHPBar())
	b.WriteString("\n\n")

	// Combat stats
	b.WriteString(c.renderCombatStats())
	b.WriteString("\n")

	// Weapon info
	b.WriteString(c.renderWeaponInfo())
	b.WriteString("\n")

	// Buffs (if any)
	if len(c.player.Buffs) > 0 {
		b.WriteString(c.renderBuffs())
	}

	// ASCII Art placeholder
	b.WriteString("\n")
	b.WriteString(c.renderASCIIArt())

	return c.applyPanelStyle(b.String())
}

// renderBasicStats renders HP, Gold.
func (c Character) renderBasicStats() string {
	var b strings.Builder

	// HP: 100/100
	hpLabel := styles.StatLabelStyle.Render("HP: ")
	hpValue := c.renderHPValue()
	b.WriteString(hpLabel + hpValue + "\n")

	// Gold: 500 G
	goldLabel := styles.StatLabelStyle.Render("Gold: ")
	goldValue := styles.GoldStyle.Render(fmt.Sprintf("%d G", c.player.Gold))
	b.WriteString(goldLabel + goldValue)

	return b.String()
}

// renderHPValue with color based on HP %.
func (c Character) renderHPValue() string {
	hp := c.player.CurrentHP
	maxHP := c.player.MaxHP
	pct := float64(hp) / float64(maxHP)

	hpStr := fmt.Sprintf("%d/%d", hp, maxHP)

	if pct <= HPLowThreshold {
		return styles.HPLowStyle.Render(hpStr)
	}
	return styles.HPFullStyle.Render(hpStr)
}

// renderHPBar renders visual HP bar.
func (c Character) renderHPBar() string {
	hp := c.player.CurrentHP
	maxHP := c.player.MaxHP
	pct := float64(hp) / float64(maxHP)

	barWidth := c.width - HPBarPadding
	if barWidth < HPBarMinWidth {
		barWidth = HPBarMinWidth
	}
	if barWidth > HPBarMaxWidth {
		barWidth = HPBarMaxWidth
	}

	filled := int(pct * float64(barWidth))
	if filled < 0 {
		filled = 0
	}

	bar := strings.Repeat("█", filled) + strings.Repeat("░", barWidth-filled)

	if pct <= HPLowThreshold {
		return styles.HPLowStyle.Render("[" + bar + "]")
	}
	return styles.HPFullStyle.Render("[" + bar + "]")
}

// renderCombatStats renders ATK, DEF.
func (c Character) renderCombatStats() string {
	var b strings.Builder

	// ATK: 25
	atkLabel := styles.StatLabelStyle.Render("ATK: ")
	atkValue := styles.StatValueStyle.Render(fmt.Sprintf("%d", c.player.GetAttack()))
	b.WriteString(atkLabel + atkValue + "\n")

	// DEF: 10
	defLabel := styles.StatLabelStyle.Render("DEF: ")
	defValue := styles.StatValueStyle.Render(fmt.Sprintf("%d", c.player.GetDefense()))
	b.WriteString(defLabel + defValue)

	return b.String()
}

// renderWeaponInfo renders equipped weapon.
func (c Character) renderWeaponInfo() string {
	var b strings.Builder

	b.WriteString(styles.StatLabelStyle.Render("Weapon: "))

	if c.player.EquippedWeapon != nil {
		w := c.player.EquippedWeapon
		name := w.DisplayName()

		// Color based on element
		switch w.Element {
		case domain.ElementFire:
			b.WriteString(styles.FireStyle.Render(name))
		case domain.ElementWater:
			b.WriteString(styles.WaterStyle.Render(name))
		case domain.ElementEarth:
			b.WriteString(styles.EarthStyle.Render(name))
		case domain.ElementWind:
			b.WriteString(styles.WindStyle.Render(name))
		default:
			b.WriteString(styles.StatValueStyle.Render(name))
		}

		// Damage
		b.WriteString(styles.StatLabelStyle.Render(" ("))
		b.WriteString(styles.DamageStyle.Render(fmt.Sprintf("+%d", w.Damage())))
		b.WriteString(styles.StatLabelStyle.Render(")"))
	} else {
		b.WriteString(styles.DisabledItemStyle.Render("None"))
	}

	return b.String()
}

// renderBuffs renders active buffs.
func (c Character) renderBuffs() string {
	var b strings.Builder

	b.WriteString(styles.StatLabelStyle.Render("Buffs: "))

	buffs := []string{}
	for _, buff := range c.player.Buffs {
		if buff.TurnsRemaining > 0 {
			buffStr := fmt.Sprintf("%s (%d)", buffTypeName(buff.Type), buff.TurnsRemaining)
			buffs = append(buffs, styles.BuffStyle.Render(buffStr))
		}
	}

	if len(buffs) > 0 {
		b.WriteString(strings.Join(buffs, ", "))
	} else {
		b.WriteString(styles.DisabledItemStyle.Render("None"))
	}

	return b.String()
}

// renderASCIIArt renders character ASCII art.
func (c Character) renderASCIIArt() string {
	// Simple warrior ASCII art
	art := `
   O
  /|\
  / \
`
	return styles.StatLabelStyle.Render(art)
}

// buffTypeName returns string representation of buff type.
func buffTypeName(t domain.BuffType) string {
	switch t {
	case domain.BuffAttack:
		return "ATK UP"
	case domain.BuffDefense:
		return "DEF UP"
	default:
		return "Unknown"
	}
}

// applyPanelStyle applies panel border style.
func (c Character) applyPanelStyle(content string) string {
	style := styles.BasePanelStyle
	if c.focused {
		style = styles.ActivePanelStyle
	}

	return style.
		Width(c.width).
		Height(c.height).
		Render(content)
}
