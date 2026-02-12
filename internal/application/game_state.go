// Package application provides use cases and orchestration.
//
// This layer:
// - Manages game state
// - Coordinates domain operations
// - Emits events for UI to consume
// - Does NOT know about UI implementation
package application

// GameScreen represents the currently active screen.
type GameScreen int

const (
	ScreenMenu GameScreen = iota
	ScreenStatus
	ScreenBattle
	ScreenShop
	ScreenInventory
	ScreenSettings
	ScreenGameOver
)

// String returns screen name.
func (s GameScreen) String() string {
	switch s {
	case ScreenStatus:
		return "Status"
	case ScreenBattle:
		return "Battle"
	case ScreenShop:
		return "Shop"
	case ScreenInventory:
		return "Inventory"
	case ScreenSettings:
		return "Settings"
	case ScreenGameOver:
		return "Game Over"
	default:
		return "Menu"
	}
}

// MenuItem represents a menu option.
type MenuItem struct {
	ID      string
	Label   string
	Enabled bool
}

// GameState manages the application state.
type GameState struct {
	CurrentScreen GameScreen
	MenuIndex     int
	MenuItems     []MenuItem
	Running       bool

	// Player name (set during intro)
	PlayerName string

	// Events channel for UI updates
	Events chan GameEvent
}

// GameEvent represents a state change notification.
type GameEvent struct {
	Type    EventType
	Message string
	Data    interface{}
}

// EventType categorizes events.
type EventType int

const (
	EventNone EventType = iota
	EventScreenChanged
	EventMenuChanged
	EventMessage
	EventBattleStart
	EventBattleEnd
	EventQuit
)

// NewGameState creates a new game state.
func NewGameState() *GameState {
	return &GameState{
		CurrentScreen: ScreenMenu,
		MenuIndex:     0,
		MenuItems: []MenuItem{
			{ID: "battle", Label: "Battle", Enabled: true},
			{ID: "shop", Label: "Shop", Enabled: true},
			{ID: "inventory", Label: "Inventory", Enabled: true},
			{ID: "status", Label: "Status", Enabled: true},
			{ID: "settings", Label: "Settings", Enabled: true},
		},
		Running: true,
		Events:  make(chan GameEvent, 16),
	}
}

// Emit sends an event to listeners.
func (g *GameState) Emit(event GameEvent) {
	select {
	case g.Events <- event:
	default:
		// Channel full, drop event (non-blocking)
	}
}

// MenuUp moves menu selection up.
func (g *GameState) MenuUp() {
	if len(g.MenuItems) == 0 {
		return
	}
	g.MenuIndex--
	if g.MenuIndex < 0 {
		g.MenuIndex = len(g.MenuItems) - 1
	}
	g.Emit(GameEvent{Type: EventMenuChanged})
}

// MenuDown moves menu selection down.
func (g *GameState) MenuDown() {
	if len(g.MenuItems) == 0 {
		return
	}
	g.MenuIndex++
	if g.MenuIndex >= len(g.MenuItems) {
		g.MenuIndex = 0
	}
	g.Emit(GameEvent{Type: EventMenuChanged})
}

// MenuSelect activates the current menu item.
func (g *GameState) MenuSelect() {
	if g.MenuIndex >= len(g.MenuItems) {
		return
	}

	item := g.MenuItems[g.MenuIndex]
	if !item.Enabled {
		return
	}

	switch item.ID {
	case "battle":
		g.CurrentScreen = ScreenBattle
	case "shop":
		g.CurrentScreen = ScreenShop
	case "inventory":
		g.CurrentScreen = ScreenInventory
	case "status":
		g.CurrentScreen = ScreenStatus
	case "settings":
		g.CurrentScreen = ScreenSettings
	}

	g.Emit(GameEvent{Type: EventScreenChanged, Message: item.Label})
}

// Quit signals the game to exit.
func (g *GameState) Quit() {
	g.Running = false
	g.Emit(GameEvent{Type: EventQuit})
}

// ReturnToMenu resets to main menu screen.
func (g *GameState) ReturnToMenu() {
	g.CurrentScreen = ScreenMenu
	g.Emit(GameEvent{Type: EventScreenChanged, Message: "Menu"})
}

// IsQuitting returns true if game is exiting.
func (g *GameState) IsQuitting() bool {
	return !g.Running
}
