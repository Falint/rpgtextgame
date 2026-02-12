// Package tui provides the BubbleTea-based terminal UI.
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
type AppModel struct {
	// Domain & Services
	player        *domain.Player
	shopService   *application.ShopService
	battleService *application.BattleService

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

// NewAppModel creates the initial model.
func NewAppModel(
	player *domain.Player,
	shopService *application.ShopService,
	battleService *application.BattleService,
	weapons *registry.WeaponRegistry,
	items *registry.ItemRegistry,
	monsters *registry.MonsterRegistry,
	upgrades *registry.UpgradeRegistry,
) AppModel {
	return AppModel{
		player:        player,
		shopService:   shopService,
		battleService: battleService,
		weapons:       weapons,
		items:         items,
		monsters:      monsters,
		upgrades:      upgrades,

		menu:         components.NewMenu(),
		character:    components.NewCharacter(player),
		activeScreen: nil, // Initially empty or welcome

		focusedPanel: PanelMenu,
	}
}

// Init initializes the model.
func (m AppModel) Init() tea.Cmd {
	return nil
}
