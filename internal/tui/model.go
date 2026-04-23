// Package tui implements the terminal user interface using BubbleTea.
// This is the presentation layer of the Clean Architecture, depending on
// all other layers (domain, application, infrastructure) via injection.
//
// ARCHITECTURE: Elm Architecture (Model-Update-View)
// - Model (AppModel): holds all application state
// - Update: processes messages, returns updated model + commands
// - View: renders model state to terminal string
package tui

import (
	tea "github.com/charmbracelet/bubbletea"
	"github.com/tenyom/textrpg-tui/internal/application"
	"github.com/tenyom/textrpg-tui/internal/domain"
	"github.com/tenyom/textrpg-tui/internal/infrastructure/registry"
	"github.com/tenyom/textrpg-tui/internal/tui/components"
	"github.com/tenyom/textrpg-tui/internal/tui/screens"
)

// AppModel is the main BubbleTea model.
// It holds all services, registries, and UI components, and routes
// input to the active screen or menu.
type AppModel struct {
	// Domain & Services
	player        *domain.Player
	shopService   *application.ShopService
	battleService *application.BattleService
	itemService   *application.ItemService // Shared item effect processor

	// Registries
	weapons  *registry.WeaponRegistry
	items    *registry.ItemRegistry
	monsters *registry.MonsterRegistry
	upgrades *registry.UpgradeRegistry

	// Components
	menu      components.Menu
	character components.Character

	// Active Screen (Block 3)
	activeScreen screens.Screen

	// State
	width        int
	height       int
	focusedPanel int
}

// NewAppModel creates the initial model with all services and registries injected.
func NewAppModel(
	player *domain.Player,
	shopService *application.ShopService,
	battleService *application.BattleService,
	itemService *application.ItemService,
	weapons *registry.WeaponRegistry,
	items *registry.ItemRegistry,
	monsters *registry.MonsterRegistry,
	upgrades *registry.UpgradeRegistry,
) AppModel {
	return AppModel{
		player:        player,
		shopService:   shopService,
		battleService: battleService,
		itemService:   itemService,
		weapons:       weapons,
		items:         items,
		monsters:      monsters,
		upgrades:      upgrades,

		menu:         components.NewMenu(),
		character:    components.NewCharacter(player),
		activeScreen: nil,

		focusedPanel: PanelMenu,
	}
}

// Init initializes the model.
func (m AppModel) Init() tea.Cmd {
	return nil
}
