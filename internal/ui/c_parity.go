package ui

import (
	"fmt"
	"strings"

	"github.com/gdamore/tcell/v2"
	"github.com/tenyom/textrpg-tui/internal/domain"
)

// CMenuOutput provides exact C main.c output parity.
// All functions here produce byte-identical output to C version.
//
// Reference: src/main.c print_main_menu() lines 33-46
// Reference: src/core/player.c player_print_status() lines 276-293
// Reference: src/utils/utils.c print_header(), print_separator(), print_centered()

// Constants matching C config.h
const (
	cMenuWidth = 17 // Length of "=== MAIN MENU ==="
)

// RenderCMainMenu renders EXACTLY what C's print_main_menu() outputs.
// Block 3 will display this when on main menu screen.
//
// C Output (byte-exact):
//
//	=== MAIN MENU ===
//	1. Battle (Normal)
//	2. Battle (Elite)
//	3. Battle (BOSS)
//	4. Shop
//	5. Upgrade Weapons
//	6. Equip Weapon
//	7. Inventory
//	8. Status
//	9. Exit
//	=================
//	Choice:
func (a *App) RenderCMainMenu(x, y int) {
	lines := []string{
		"=== MAIN MENU ===",
		"1. Battle (Normal)",
		"2. Battle (Elite)",
		"3. Battle (BOSS)",
		"4. Shop",
		"5. Upgrade Weapons",
		"6. Equip Weapon",
		"7. Inventory",
		"8. Status",
		"9. Exit",
		"=================",
		"Choice: ",
	}

	for i, line := range lines {
		a.renderer.DrawText(x, y+i, line, tcell.StyleDefault)
	}
}

// RenderCPlayerStatus renders EXACTLY what C's player_print_status() outputs.
//
// C Output format (byte-exact):
// [Hero] HP: 100/100 | ATK: 25 | DEF: 5 | Gold: 150 | Iron Sword (+10) [Fire]
//
// Reference: src/core/player.c lines 280-288
func (a *App) RenderCPlayerStatus(x, y int, p *domain.Player) {
	// Base format: [name] HP: cur/max | ATK: atk | DEF: def | Gold: gold
	status := fmt.Sprintf("[%s] HP: %d/%d | ATK: %d | DEF: %d | Gold: %d",
		p.Name,
		p.CurrentHP,
		p.MaxHP,
		p.GetAttack(),
		p.GetDefense(),
		p.Gold)

	// Append weapon if equipped (and not fists)
	if p.EquippedWeapon != nil {
		status += " | "
		status += a.formatCWeapon(p.EquippedWeapon)
	}

	a.renderer.DrawText(x, y, status, tcell.StyleDefault)

	// Buffs on next line (if any)
	if len(p.Buffs) > 0 {
		buffStr := a.formatCBuffs(p)
		a.renderer.DrawText(x, y+1, buffStr, tcell.StyleDefault)
	}
}

// formatCWeapon formats weapon EXACTLY like C's weapon_print().
// C Output: "Iron Sword (+10) [Fire]"
func (a *App) formatCWeapon(w *domain.Weapon) string {
	dmg := w.Damage()
	result := fmt.Sprintf("%s (+%d)", w.Name, dmg)

	if w.Element != domain.ElementNone {
		result += fmt.Sprintf(" [%s]", w.Element.String())
	}

	return result
}

// formatCBuffs formats buffs EXACTLY like C's player_print_buffs().
// C Output: "  Buffs: [ATK+25% 3t] [DEF+25% 2t]"
func (a *App) formatCBuffs(p *domain.Player) string {
	if len(p.Buffs) == 0 {
		return ""
	}

	result := "  Buffs: "
	parts := make([]string, 0, len(p.Buffs))

	for _, buff := range p.Buffs {
		var buffStr string
		switch buff.Type {
		case domain.BuffAttack:
			buffStr = fmt.Sprintf("[ATK+%d%% %dt]", buff.Value, buff.TurnsRemaining)
		case domain.BuffDefense:
			buffStr = fmt.Sprintf("[DEF+%d%% %dt]", buff.Value, buff.TurnsRemaining)
		}
		if buffStr != "" {
			parts = append(parts, buffStr)
		}
	}

	result += strings.Join(parts, " ")
	return result
}

// RenderCHeader renders EXACTLY like C's print_header().
// C Output:
// ========================================
//
//	TITLE
//
// ========================================
func (a *App) RenderCHeader(x, y int, title string, width int) int {
	// Top separator
	sep := strings.Repeat("=", width)
	a.renderer.DrawText(x, y, sep, tcell.StyleDefault)

	// Centered title
	padding := (width - len(title)) / 2
	if padding < 0 {
		padding = 0
	}
	centeredTitle := strings.Repeat(" ", padding) + title
	a.renderer.DrawText(x, y+1, centeredTitle, tcell.StyleDefault)

	// Bottom separator
	a.renderer.DrawText(x, y+2, sep, tcell.StyleDefault)

	return 3 // Lines used
}

// RenderCSeparator renders EXACTLY like C's print_separator().
func (a *App) RenderCSeparator(x, y, width int, ch rune) {
	sep := strings.Repeat(string(ch), width)
	a.renderer.DrawText(x, y, sep, tcell.StyleDefault)
}
