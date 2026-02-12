// Package ui provides the Terminal User Interface.
//
// This is a Pure Go implementation using tcell.
// NO CGO, NO C CODE.
//
// Architecture:
// - screen.go: Terminal abstraction
// - input.go: Key event handling
// - renderer.go: Drawing primitives
// - layout.go: Panel calculations
// - app.go: Main application loop
package ui

import (
	"fmt"

	"github.com/gdamore/tcell/v2"
	"github.com/tenyom/textrpg-tui/internal/application"
	"github.com/tenyom/textrpg-tui/internal/domain"
	"github.com/tenyom/textrpg-tui/internal/infrastructure/registry"
)

// App is the main TUI application.
type App struct {
	screen   *Screen
	renderer *Renderer
	state    *application.GameState
	player   *domain.Player
	layout   Layout
	config   LayoutConfig

	// Services
	battle   *application.BattleService
	shop     *application.ShopService
	monsters *registry.MonsterRegistry

	// UI state
	battleMenuIdx int
	shopMenuIdx   int
	invMenuIdx    int
	running       bool
}

// NewApp creates a new TUI application.
func NewApp(
	state *application.GameState,
	player *domain.Player,
	battle *application.BattleService,
	shop *application.ShopService,
	monsters *registry.MonsterRegistry,
) (*App, error) {
	screen, err := NewScreen()
	if err != nil {
		return nil, fmt.Errorf("screen init: %w", err)
	}

	app := &App{
		screen:   screen,
		renderer: NewRenderer(screen),
		state:    state,
		player:   player,
		battle:   battle,
		shop:     shop,
		monsters: monsters,
		config:   DefaultLayoutConfig(),
		running:  true,
	}

	app.updateLayout()
	return app, nil
}

// Run starts the main application loop.
func (a *App) Run() error {
	a.screen.HideCursor()
	defer a.screen.Close()

	for a.running && !a.state.IsQuitting() {
		a.updateLayout()
		a.render()
		a.screen.Show()

		ev := a.screen.PollEvent()
		a.handleEvent(ev)
	}

	return nil
}

// updateLayout recalculates panel dimensions.
func (a *App) updateLayout() {
	w, h := a.screen.Size()
	a.layout = NewLayout(w, h, a.config)
}

// handleEvent processes tcell events.
func (a *App) handleEvent(ev tcell.Event) {
	switch ev := ev.(type) {
	case *tcell.EventKey:
		a.handleKeyEvent(ConvertTcellKey(ev))
	case *tcell.EventResize:
		a.screen.Sync()
		a.updateLayout()
	}
}

// handleKeyEvent processes keyboard input.
func (a *App) handleKeyEvent(event KeyEvent) {
	// Global quit keys
	if event.Key == KeyCtrlC || event.Key == KeyCtrlQ {
		a.state.Quit()
		a.running = false
		return
	}
	if event.Key == KeyChar && (event.Char == 'q' || event.Char == 'Q') {
		if a.state.CurrentScreen == application.ScreenMenu {
			a.state.Quit()
			a.running = false
			return
		}
	}

	// Screen-specific handling
	switch a.state.CurrentScreen {
	case application.ScreenMenu:
		a.handleMenuInput(event)
	case application.ScreenBattle:
		a.handleBattleInput(event)
	case application.ScreenShop:
		a.handleShopInput(event)
	case application.ScreenInventory:
		a.handleInventoryInput(event)
	default:
		a.handleDefaultInput(event)
	}
}

func (a *App) handleMenuInput(event KeyEvent) {
	switch event.Key {
	case KeyUp:
		a.state.MenuUp()
	case KeyDown:
		a.state.MenuDown()
	case KeyEnter:
		item := a.state.MenuItems[a.state.MenuIndex]
		if item.ID == "battle" {
			a.battle.StartBattle(domain.MonsterNormal)
		}
		a.state.MenuSelect()
	case KeyChar:
		switch event.Char {
		case 'j':
			a.state.MenuDown()
		case 'k':
			a.state.MenuUp()
		}
	}
}

func (a *App) handleBattleInput(event KeyEvent) {
	if a.battle.IsBattleOver() {
		if event.Key == KeyEnter || event.Key == KeySpace {
			a.battle.EndBattle()
			a.state.ReturnToMenu()
		}
		return
	}

	switch event.Key {
	case KeyUp:
		a.battleMenuIdx--
		if a.battleMenuIdx < 0 {
			a.battleMenuIdx = 3
		}
	case KeyDown:
		a.battleMenuIdx++
		if a.battleMenuIdx > 3 {
			a.battleMenuIdx = 0
		}
	case KeyEnter:
		switch a.battleMenuIdx {
		case 0:
			a.battle.Attack()
		case 1:
			// Use item - TODO
		case 2:
			a.battle.Attack()
		case 3:
			a.battle.TryEscape()
		}
	case KeyEscape:
		a.battle.TryEscape()
	case KeyChar:
		switch event.Char {
		case '1':
			a.battle.Attack()
		case '4':
			a.battle.TryEscape()
		case 'j':
			a.battleMenuIdx = (a.battleMenuIdx + 1) % 4
		case 'k':
			a.battleMenuIdx = (a.battleMenuIdx + 3) % 4
		}
	}
}

func (a *App) handleShopInput(event KeyEvent) {
	if event.Key == KeyEscape {
		a.state.ReturnToMenu()
	}
}

func (a *App) handleInventoryInput(event KeyEvent) {
	if event.Key == KeyEscape {
		a.state.ReturnToMenu()
	}
}

func (a *App) handleDefaultInput(event KeyEvent) {
	if event.Key == KeyEscape {
		a.state.ReturnToMenu()
	}
}

// render draws the entire UI.
func (a *App) render() {
	a.screen.Clear()

	a.renderMenuPanel()
	a.renderContentPanel()
	a.renderActionPanel()
}

func (a *App) renderMenuPanel() {
	rect := a.layout.MenuRect()
	a.renderer.DrawBoxSimple(rect.X, rect.Y, rect.Width, rect.Height, "Menu")

	// Menu items
	for i, item := range a.state.MenuItems {
		y := rect.Y + 2 + i
		if y >= rect.Y+rect.Height-1 {
			break
		}

		style := tcell.StyleDefault
		prefix := "  "
		if i == a.state.MenuIndex {
			prefix = "▸ "
			style = style.Reverse(true)
		}

		text := fmt.Sprintf("%s%s", prefix, item.Label)
		a.renderer.DrawText(rect.X+1, y, text, style)
	}

	// Help
	helpY := rect.Y + rect.Height - 5
	a.renderer.DrawText(rect.X+1, helpY, "───────────", tcell.StyleDefault)
	a.renderer.DrawText(rect.X+1, helpY+1, "[↑↓] Navigate", tcell.StyleDefault)
	a.renderer.DrawText(rect.X+1, helpY+2, "[Enter] Select", tcell.StyleDefault)
	a.renderer.DrawText(rect.X+1, helpY+3, "[Q] Quit", tcell.StyleDefault)
}

func (a *App) renderContentPanel() {
	rect := a.layout.ContentRect()
	a.renderer.DrawBoxSimple(rect.X, rect.Y, rect.Width, rect.Height, a.state.CurrentScreen.String())

	x := rect.X + 2
	switch a.state.CurrentScreen {
	case application.ScreenStatus:
		a.renderStatusContent(x, rect.Y+2)
	case application.ScreenBattle:
		a.renderBattleContent(x, rect.Y+2)
	case application.ScreenShop:
		a.renderShopContent(x, rect.Y+2)
	case application.ScreenInventory:
		a.renderInventoryContent(x, rect.Y+2)
	default:
		a.renderStatusContent(x, rect.Y+2)
	}
}

func (a *App) renderStatusContent(x, y int) {
	// ASCII character
	lines := []string{
		"    ╭─────────────╮",
		"    │   ○ ◯ ○    │",
		"    │  ╭───╮     │",
		"    │  │ ▓ │ HERO│",
		"    │  ╰─┬─╯     │",
		"    │   / \\      │",
		"    ╰─────────────╯",
	}
	for i, line := range lines {
		a.renderer.DrawText(x, y+i, line, tcell.StyleDefault)
	}

	// Stats
	y += len(lines) + 1
	a.renderer.DrawText(x, y, fmt.Sprintf("Name: %s", a.player.Name), tcell.StyleDefault)
	a.renderer.DrawText(x, y+1, fmt.Sprintf("HP: %d/%d", a.player.CurrentHP, a.player.MaxHP), tcell.StyleDefault)
	a.renderer.DrawText(x, y+2, fmt.Sprintf("ATK: %d", a.player.GetAttack()), tcell.StyleDefault)
	a.renderer.DrawText(x, y+3, fmt.Sprintf("DEF: %d", a.player.GetDefense()), tcell.StyleDefault)
	a.renderer.DrawText(x, y+4, fmt.Sprintf("Gold: %d", a.player.Gold), tcell.StyleDefault)

	if a.player.EquippedWeapon != nil {
		a.renderer.DrawText(x, y+6, fmt.Sprintf("Weapon: %s", a.player.EquippedWeapon.Name), tcell.StyleDefault)
	}
}

func (a *App) renderBattleContent(x, y int) {
	enemy := a.battle.GetEnemy()
	if enemy == nil {
		a.renderer.DrawText(x, y, "No enemy!", tcell.StyleDefault)
		return
	}

	// Enemy display
	redStyle := tcell.StyleDefault.Foreground(tcell.ColorRed)
	a.renderer.DrawText(x, y, "═══════ BATTLE ═══════", redStyle)

	// Enemy ASCII
	enemyArt := []string{
		"    ╭───────────╮",
		fmt.Sprintf("    │  %-8s │", enemy.Type.String()),
		"    │    ◉  ◉   │",
		"    │     ▼     │",
		"    │   ╭───╮   │",
		"    ╰───┴───┴───╯",
	}
	for i, line := range enemyArt {
		a.renderer.DrawText(x, y+2+i, line, tcell.StyleDefault)
	}

	// Enemy name and element
	yellowStyle := tcell.StyleDefault.Foreground(tcell.ColorYellow)
	nameY := y + 2 + len(enemyArt) + 1
	a.renderer.DrawText(x, nameY, enemy.Name, yellowStyle)
	if enemy.Element != domain.ElementNone {
		a.renderer.DrawText(x+len(enemy.Name)+1, nameY, fmt.Sprintf("[%s]", enemy.Element.String()), tcell.StyleDefault)
	}

	// HP bars
	a.renderHPBar(x, nameY+1, "Enemy", enemy.CurrentHP, enemy.MaxHP)
	a.renderHPBar(x, nameY+3, "You", a.player.CurrentHP, a.player.MaxHP)

	// Battle log
	logY := nameY + 5
	a.renderer.DrawText(x, logY, "─── Battle Log ───", tcell.StyleDefault)
	logs := a.battle.GetLog()
	start := 0
	if len(logs) > 5 {
		start = len(logs) - 5
	}
	for i, log := range logs[start:] {
		a.renderer.DrawText(x, logY+1+i, "> "+log, tcell.StyleDefault)
	}

	// Victory/Defeat message
	if a.battle.IsBattleOver() {
		msgY := logY + 7
		state := a.battle.GetState()
		switch state {
		case application.BattleWon:
			a.renderer.DrawText(x, msgY, "VICTORY! Press Enter.", tcell.StyleDefault.Foreground(tcell.ColorGreen))
		case application.BattleLost:
			a.renderer.DrawText(x, msgY, "DEFEAT! Press Enter.", tcell.StyleDefault.Foreground(tcell.ColorRed))
		case application.BattleEscaped:
			a.renderer.DrawText(x, msgY, "Escaped! Press Enter.", tcell.StyleDefault)
		}
	}
}

func (a *App) renderHPBar(x, y int, label string, current, max int) {
	barWidth := 20
	filled := 0
	if max > 0 {
		filled = int(float64(current) / float64(max) * float64(barWidth))
	}

	bar := ""
	for i := 0; i < barWidth; i++ {
		if i < filled {
			bar += "█"
		} else {
			bar += "░"
		}
	}

	text := fmt.Sprintf("%s: [%s] %d/%d", label, bar, current, max)
	a.renderer.DrawText(x, y, text, tcell.StyleDefault)
}

func (a *App) renderShopContent(x, y int) {
	yellowStyle := tcell.StyleDefault.Foreground(tcell.ColorYellow)
	a.renderer.DrawText(x, y, "═══════ SHOP ═══════", yellowStyle)
	a.renderer.DrawText(x, y+2, fmt.Sprintf("Gold: %d", a.player.Gold), tcell.StyleDefault)
	a.renderer.DrawText(x, y+4, "Press ESC to return.", tcell.StyleDefault)
}

func (a *App) renderInventoryContent(x, y int) {
	cyanStyle := tcell.StyleDefault.Foreground(tcell.ColorTeal)
	a.renderer.DrawText(x, y, "═══════ INVENTORY ═══════", cyanStyle)

	slots := a.player.Inventory.UsedSlots()
	a.renderer.DrawText(x, y+2, fmt.Sprintf("Used: %d/%d slots", slots, a.player.Inventory.MaxSlots), tcell.StyleDefault)
	a.renderer.DrawText(x, y+4, "Press ESC to return.", tcell.StyleDefault)
}

func (a *App) renderActionPanel() {
	rect := a.layout.ActionRect()
	title := a.getActionPanelTitle()
	a.renderer.DrawBoxSimple(rect.X, rect.Y, rect.Width, rect.Height, title)

	x := rect.X + 2
	switch a.state.CurrentScreen {
	case application.ScreenMenu:
		a.renderMenuActions(x, rect.Y+2)
	case application.ScreenBattle:
		a.renderBattleActions(x, rect.Y+2)
	default:
		a.renderMenuActions(x, rect.Y+2)
	}
}

func (a *App) getActionPanelTitle() string {
	switch a.state.CurrentScreen {
	case application.ScreenBattle:
		return "Combat"
	case application.ScreenShop:
		return "Shop Actions"
	case application.ScreenInventory:
		return "Item Actions"
	default:
		return "Quick Actions"
	}
}

func (a *App) renderMenuActions(x, y int) {
	// EXACT C OUTPUT PARITY
	// Reference: src/main.c print_main_menu() lines 33-46
	//
	// C Output (byte-exact):
	// === MAIN MENU ===
	// 1. Battle (Normal)
	// 2. Battle (Elite)
	// 3. Battle (BOSS)
	// 4. Shop
	// 5. Upgrade Weapons
	// 6. Equip Weapon
	// 7. Inventory
	// 8. Status
	// 9. Exit
	// =================
	// Choice:

	a.RenderCMainMenu(x, y)
}

// Battle preview in Action Panel
func (a *App) renderBattlePreview(x, y int) {
	redStyle := tcell.StyleDefault.Foreground(tcell.ColorRed)
	a.renderer.DrawText(x, y, "⚔️  BATTLE", redStyle)
	a.renderer.DrawText(x, y+2, "───────────────", tcell.StyleDefault)
	a.renderer.DrawText(x, y+3, "Fight monsters", tcell.StyleDefault)
	a.renderer.DrawText(x, y+4, "to earn gold", tcell.StyleDefault)
	a.renderer.DrawText(x, y+5, "and items!", tcell.StyleDefault)
	a.renderer.DrawText(x, y+7, "───────────────", tcell.StyleDefault)

	// Player quick stats
	a.renderer.DrawText(x, y+8, fmt.Sprintf("HP: %d/%d", a.player.CurrentHP, a.player.MaxHP), tcell.StyleDefault)
	a.renderer.DrawText(x, y+9, fmt.Sprintf("ATK: %d", a.player.GetAttack()), tcell.StyleDefault)

	a.renderer.DrawText(x, y+11, "[Enter] Start", tcell.StyleDefault.Bold(true))
}

// Shop preview in Action Panel
func (a *App) renderShopPreview(x, y int) {
	yellowStyle := tcell.StyleDefault.Foreground(tcell.ColorYellow)
	a.renderer.DrawText(x, y, "🛒 SHOP", yellowStyle)
	a.renderer.DrawText(x, y+2, "───────────────", tcell.StyleDefault)
	a.renderer.DrawText(x, y+3, "Buy weapons", tcell.StyleDefault)
	a.renderer.DrawText(x, y+4, "and items!", tcell.StyleDefault)
	a.renderer.DrawText(x, y+6, "───────────────", tcell.StyleDefault)

	// Player gold
	goldStyle := tcell.StyleDefault.Foreground(tcell.ColorYellow)
	a.renderer.DrawText(x, y+7, fmt.Sprintf("Gold: %d", a.player.Gold), goldStyle)

	// Equipped weapon
	if a.player.EquippedWeapon != nil {
		a.renderer.DrawText(x, y+9, "Weapon:", tcell.StyleDefault)
		a.renderer.DrawText(x, y+10, a.player.EquippedWeapon.Name, tcell.StyleDefault)
	}

	a.renderer.DrawText(x, y+12, "[Enter] Open", tcell.StyleDefault.Bold(true))
}

// Inventory preview in Action Panel
func (a *App) renderInventoryPreview(x, y int) {
	cyanStyle := tcell.StyleDefault.Foreground(tcell.ColorTeal)
	a.renderer.DrawText(x, y, "📦 INVENTORY", cyanStyle)
	a.renderer.DrawText(x, y+2, "───────────────", tcell.StyleDefault)

	// Slot usage
	used := a.player.Inventory.UsedSlots()
	max := a.player.Inventory.MaxSlots
	a.renderer.DrawText(x, y+3, fmt.Sprintf("Slots: %d/%d", used, max), tcell.StyleDefault)

	// Item counts
	weapons := len(a.player.Inventory.GetWeapons())
	items := len(a.player.Inventory.GetItems())
	a.renderer.DrawText(x, y+5, fmt.Sprintf("Weapons: %d", weapons), tcell.StyleDefault)
	a.renderer.DrawText(x, y+6, fmt.Sprintf("Items: %d", items), tcell.StyleDefault)

	a.renderer.DrawText(x, y+8, "───────────────", tcell.StyleDefault)
	a.renderer.DrawText(x, y+9, "[Enter] Open", tcell.StyleDefault.Bold(true))
}

// Status preview in Action Panel
func (a *App) renderStatusPreview(x, y int) {
	greenStyle := tcell.StyleDefault.Foreground(tcell.ColorGreen)
	a.renderer.DrawText(x, y, "📊 STATUS", greenStyle)
	a.renderer.DrawText(x, y+2, "───────────────", tcell.StyleDefault)

	// Full player stats
	a.renderer.DrawText(x, y+3, fmt.Sprintf("Name: %s", a.player.Name), tcell.StyleDefault)
	a.renderer.DrawText(x, y+4, fmt.Sprintf("HP: %d/%d", a.player.CurrentHP, a.player.MaxHP), tcell.StyleDefault)
	a.renderer.DrawText(x, y+5, fmt.Sprintf("ATK: %d", a.player.GetAttack()), tcell.StyleDefault)
	a.renderer.DrawText(x, y+6, fmt.Sprintf("DEF: %d", a.player.GetDefense()), tcell.StyleDefault)
	a.renderer.DrawText(x, y+7, fmt.Sprintf("Gold: %d", a.player.Gold), tcell.StyleDefault)

	// Buffs
	if len(a.player.Buffs) > 0 {
		a.renderer.DrawText(x, y+9, fmt.Sprintf("Buffs: %d active", len(a.player.Buffs)), tcell.StyleDefault)
	}

	a.renderer.DrawText(x, y+11, "[Enter] View", tcell.StyleDefault.Bold(true))
}

// Settings preview in Action Panel
func (a *App) renderSettingsPreview(x, y int) {
	magentaStyle := tcell.StyleDefault.Foreground(tcell.ColorPurple)
	a.renderer.DrawText(x, y, "⚙️  SETTINGS", magentaStyle)
	a.renderer.DrawText(x, y+2, "───────────────", tcell.StyleDefault)
	a.renderer.DrawText(x, y+3, "Game options", tcell.StyleDefault)
	a.renderer.DrawText(x, y+4, "and controls.", tcell.StyleDefault)
	a.renderer.DrawText(x, y+6, "───────────────", tcell.StyleDefault)
	a.renderer.DrawText(x, y+7, "[Enter] Open", tcell.StyleDefault.Bold(true))
}

func (a *App) renderBattleActions(x, y int) {
	redStyle := tcell.StyleDefault.Foreground(tcell.ColorRed)
	a.renderer.DrawText(x, y, "COMBAT MODE", redStyle)
	a.renderer.DrawText(x, y+2, "───────────────", tcell.StyleDefault)

	options := []string{"Attack", "Use Item", "Defend", "Run Away"}
	for i, opt := range options {
		style := tcell.StyleDefault
		prefix := "  "
		if i == a.battleMenuIdx {
			prefix = "▸ "
			style = style.Reverse(true)
		}
		text := fmt.Sprintf("%s[%d] %s", prefix, i+1, opt)
		a.renderer.DrawText(x, y+4+i, text, style)
	}

	a.renderer.DrawText(x, y+9, "───────────────", tcell.StyleDefault)
	a.renderer.DrawText(x, y+10, "[ESC] Retreat", tcell.StyleDefault)
}
