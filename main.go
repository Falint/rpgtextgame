// TextRPG — Production Build (Go/BubbleTea)
//
// Entry point at project root so the binary can be built with:
//
//	go build -o textrpg
//
// And executed directly:
//
//	./textrpg
package main

import (
	"fmt"
	"os"

	tea "github.com/charmbracelet/bubbletea"
	"github.com/tenyom/textrpg-tui/internal/application"
	"github.com/tenyom/textrpg-tui/internal/domain"
	"github.com/tenyom/textrpg-tui/internal/infrastructure/registry"
	"github.com/tenyom/textrpg-tui/internal/tui"
)

func main() {
	// 1. Initialize Infrastructure (Registries)
	monsters := registry.NewMonsterRegistry()
	weapons := registry.NewWeaponRegistry()
	items := registry.NewItemRegistry()
	upgrades := registry.NewUpgradeRegistry()

	// 2. Initialize Domain (Player)
	player := domain.NewPlayer("Hero", domain.DefaultPlayerConfig())

	// Give starting items
	giveStartingItems(player, items, weapons)

	// 3. Initialize Application Services
	itemService := application.NewItemService(player)
	battleService := application.NewBattleService(player, monsters, items, itemService)
	shopService := application.NewShopService(player, weapons, items)

	// 4. Initialize UI (BubbleTea)
	model := tui.NewAppModel(
		player,
		shopService,
		battleService,
		itemService,
		weapons,
		items,
		monsters,
		upgrades,
	)

	// 5. Run Application
	p := tea.NewProgram(model, tea.WithAltScreen())
	if _, err := p.Run(); err != nil {
		fmt.Fprintf(os.Stderr, "Error running game: %v\n", err)
		os.Exit(1)
	}
}

// giveStartingItems gives player initial equipment matching C version.
func giveStartingItems(player *domain.Player, items *registry.ItemRegistry, weapons *registry.WeaponRegistry) {
	// Starting potion (x3)
	if tmpl := items.GetByID("small_potion"); tmpl != nil {
		player.Inventory.AddItem(tmpl.ToItem(), 3)
	}

	// Starting weapon (iron sword)
	if tmpl := weapons.GetByID("iron_sword"); tmpl != nil {
		sword := tmpl.ToWeapon()
		player.Inventory.AddWeapon(sword)
		player.EquipWeapon(sword)
	}
}
