// battle.go implements the turn-based combat screen.
// Renders enemy status, combat log, and action menu (Attack/Item/Run).
// Delegates all combat logic to BattleService.
package screens

import (
	"fmt"
	"strings"

	"github.com/charmbracelet/bubbles/key"
	tea "github.com/charmbracelet/bubbletea"
	"github.com/tenyom/textrpg-tui/internal/application"
	"github.com/tenyom/textrpg-tui/internal/domain"
	"github.com/tenyom/textrpg-tui/internal/tui/styles"
)

// -----------------------------------------------------------------------------
// Battle Screen Constants
// -----------------------------------------------------------------------------

const (
	battleLogDisplayLimit = 8 // Max battle log lines shown
	battleMainActionCount = 3 // Number of main battle actions
)

// Pre-allocated key bindings to avoid allocations on every keypress.
var (
	battleKeyUp    = key.NewBinding(key.WithKeys("up", "k"))
	battleKeyDown  = key.NewBinding(key.WithKeys("down", "j"))
	battleKeyEnter = key.NewBinding(key.WithKeys("enter"))
	battleKeyEsc   = key.NewBinding(key.WithKeys("esc"))
)

// BattleMode represents current battle interaction state.
type BattleMode int

const (
	BattleModeMain BattleMode = iota
	BattleModeItemSelect
	BattleModeResult // Won/Lost/Escaped
)

// BattleScreen implements the battle interface.
type BattleScreen struct {
	service *application.BattleService
	player  *domain.Player

	mode   BattleMode
	cursor int
	width  int
	height int
}

// NewBattleScreen creates a new battle screen.
func NewBattleScreen(service *application.BattleService, player *domain.Player) *BattleScreen {
	return &BattleScreen{
		service: service,
		player:  player,
		mode:    BattleModeMain,
		cursor:  0,
	}
}

// Title returns screen title.
func (s *BattleScreen) Title() string {
	return "BATTLE"
}

// SetSize updates dimensions.
func (s *BattleScreen) SetSize(width, height int) {
	s.width = width
	s.height = height
}

// Update handles input.
func (s *BattleScreen) Update(msg tea.Msg) tea.Cmd {
	switch msg := msg.(type) {
	case tea.KeyMsg:
		// If battle over, Enter/Esc exits back to main menu
		if s.mode == BattleModeResult {
			if msg.Type == tea.KeyEnter || msg.Type == tea.KeyEsc {
				s.service.EndBattle()
				return EndScreenCmd()
			}
			return nil
		}

		switch {
		case key.Matches(msg, battleKeyUp):
			if s.cursor > 0 {
				s.cursor--
			}
		case key.Matches(msg, battleKeyDown):
			s.cursor++
			s.clampCursor(battleMainActionCount)
		case key.Matches(msg, battleKeyEnter):
			return s.handleSelect()
		case key.Matches(msg, battleKeyEsc):
			if s.mode == BattleModeItemSelect {
				s.mode = BattleModeMain
				s.cursor = 0
			}
		case msg.String() >= "1" && msg.String() <= "3":
			if s.mode == BattleModeMain {
				idx := int(msg.String()[0] - '1')
				s.cursor = idx
				return s.handleSelect()
			}
		}
	}
	return nil
}

// handleSelect processes selection and returns a command if navigation changes.
func (s *BattleScreen) handleSelect() tea.Cmd {
	switch s.mode {
	case BattleModeMain:
		switch s.cursor {
		case 0: // Attack
			s.service.Attack()
			s.checkBattleState()
		case 1: // Use Item
			s.mode = BattleModeItemSelect
			s.cursor = 0
		case 2: // Run
			s.service.TryEscape()
			s.checkBattleState()
		}

	case BattleModeItemSelect:
		s.handleUseItem()
	}
	return nil
}

// handleUseItem finds and uses the selected consumable from inventory.
func (s *BattleScreen) handleUseItem() {
	consumables := s.player.Inventory.GetConsumables()
	if len(consumables) == 0 {
		s.mode = BattleModeMain
		return
	}

	s.clampCursor(len(consumables))

	// Find actual slot index for the Nth consumable
	targetIdx := s.findConsumableSlotIndex(s.cursor)

	if targetIdx != -1 {
		if s.service.UseItem(targetIdx) {
			s.mode = BattleModeMain
			s.checkBattleState()
		}
	}
}

// findConsumableSlotIndex returns the inventory slot index of the Nth consumable.
func (s *BattleScreen) findConsumableSlotIndex(n int) int {
	count := 0
	for i := 0; i < domain.MaxInventorySlots; i++ {
		slot := s.player.Inventory.GetSlot(i)
		if slot != nil && slot.Type == domain.SlotItem && slot.Item.Category == domain.ItemConsumable {
			if count == n {
				return i
			}
			count++
		}
	}
	return -1
}

// checkBattleState updates mode if battle ended.
func (s *BattleScreen) checkBattleState() {
	if s.service.IsBattleOver() {
		s.mode = BattleModeResult
	}
}

// View renders the screen.
func (s *BattleScreen) View() string {
	var b strings.Builder

	// Header matching C
	b.WriteString(styles.SectionHeaderStyle.Render("=== BATTLE! ==="))
	b.WriteString("\n\n")

	enemy := s.service.GetEnemy()
	if enemy == nil {
		return "No active battle."
	}

	// Enemy Status
	b.WriteString(s.renderEnemyStatus(enemy))
	b.WriteString("\n")
	b.WriteString(styles.SeparatorStyle.Render(strings.Repeat("-", s.width-4)))
	b.WriteString("\n")

	// Battle Log
	b.WriteString(s.renderLog())
	b.WriteString("\n")

	// Menu / Result
	if s.mode == BattleModeResult {
		b.WriteString(s.renderResult())
	} else if s.mode == BattleModeItemSelect {
		b.WriteString(s.renderItemSelect())
	} else {
		b.WriteString(s.renderMainMenu())
	}

	return b.String()
}

func (s *BattleScreen) renderEnemyStatus(e *domain.Enemy) string {
	var b strings.Builder

	name := e.Name
	if e.Type == domain.MonsterElite {
		name = styles.CategoryMaterial.Render("[ELITE] " + name) // Reuse a style
	} else if e.Type == domain.MonsterBoss {
		name = styles.CategoryMaterial.Render("[BOSS] " + name)
	}

	b.WriteString(fmt.Sprintf("%s\n", styles.EnemyNameStyle.Render(name)))

	// HP Bar for enemy
	hpPct := float64(e.CurrentHP) / float64(e.MaxHP)
	hpStr := fmt.Sprintf("HP: %d/%d", e.CurrentHP, e.MaxHP)

	if hpPct <= 0.3 {
		b.WriteString(styles.HPLowStyle.Render(hpStr))
	} else {
		b.WriteString(hpStr)
	}

	// Element
	if e.Element != domain.ElementNone {
		b.WriteString(fmt.Sprintf(" [%s]", e.Element.String()))
	}
	b.WriteString("\n")

	return b.String()
}

func (s *BattleScreen) renderLog() string {
	var b strings.Builder

	logs := s.service.GetLog()
	// Show last 5 logs or so depending on space
	start := 0
	if len(logs) > battleLogDisplayLimit {
		start = len(logs) - battleLogDisplayLimit
	}

	for i := start; i < len(logs); i++ {
		b.WriteString(fmt.Sprintf("> %s\n", logs[i]))
	}

	if len(logs) == 0 {
		b.WriteString(styles.DimItemStyle.Render("(Battle started...)"))
		b.WriteString("\n")
	}

	return b.String()
}

func (s *BattleScreen) renderMainMenu() string {
	var b strings.Builder

	b.WriteString(styles.SeparatorStyle.Render("--- Actions ---"))
	b.WriteString("\n")

	items := []string{
		"1. Attack",
		"2. Use Item",
		"3. Run",
	}

	for i, item := range items {
		if i == s.cursor {
			b.WriteString(styles.SelectedItemStyle.Render(item))
		} else {
			b.WriteString(styles.NormalItemStyle.Render(item))
		}
		b.WriteString("\n")
	}

	b.WriteString("Choice: ")
	return b.String()
}

func (s *BattleScreen) renderItemSelect() string {
	var b strings.Builder

	b.WriteString(styles.SeparatorStyle.Render("--- Select Item ---"))
	b.WriteString("\n")

	consumables := s.player.Inventory.GetConsumables()
	if len(consumables) == 0 {
		b.WriteString(styles.DisabledItemStyle.Render("  No items!"))
		b.WriteString("\n")
		b.WriteString(styles.HelpDescStyle.Render("[Esc] Back"))
		return b.String()
	}

	s.clampCursor(len(consumables))
	for i, slot := range consumables {
		line := fmt.Sprintf("%2d. %s x%d", i+1, slot.Item.Name, slot.Quantity)
		if i == s.cursor {
			b.WriteString(styles.SelectedItemStyle.Render(line))
		} else {
			b.WriteString(styles.NormalItemStyle.Render(line))
		}
		b.WriteString("\n")
	}

	b.WriteString("\n")
	b.WriteString(styles.HelpDescStyle.Render("[Enter] Use  [Esc] Back"))
	return b.String()
}

func (s *BattleScreen) renderResult() string {
	var b strings.Builder
	b.WriteString("\n")

	state := s.service.GetState()
	switch state {
	case application.BattleWon:
		b.WriteString(styles.SuccessStyle.Render("VICTORY!"))
		b.WriteString("\nPress [Enter] to continue...")
	case application.BattleLost:
		b.WriteString(styles.ErrorStyle.Render("DEFEAT..."))
		b.WriteString("\nPress [Enter] to continue...")
	case application.BattleEscaped:
		b.WriteString(styles.BuffStyle.Render("ESCAPED!"))
		b.WriteString("\nPress [Enter] to continue...")
	}

	return b.String()
}

func (s *BattleScreen) clampCursor(max int) {
	if max <= 0 {
		s.cursor = 0
		return
	}
	if s.cursor >= max {
		s.cursor = max - 1
	}
	if s.cursor < 0 {
		s.cursor = 0
	}
}
