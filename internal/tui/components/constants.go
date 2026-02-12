// Package components provides reusable TUI components.
// Following spiketrace architecture pattern.
package components

// -----------------------------------------------------------------------------
// Menu Item IDs (matching C main.c print_main_menu)
// -----------------------------------------------------------------------------

const (
	MenuBattleNormal = "battle_normal"
	MenuBattleElite  = "battle_elite"
	MenuBattleBoss   = "battle_boss"
	MenuShop         = "shop"
	MenuUpgrade      = "upgrade"
	MenuEquip        = "equip"
	MenuInventory    = "inventory"
	MenuStatus       = "status"
	MenuExit         = "exit"
)

// MenuItem represents a main menu option.
type MenuItem struct {
	ID    string
	Label string
	Key   string // Display key (1-9)
}

// MainMenuItems returns all 9 menu items matching C main.c
func MainMenuItems() []MenuItem {
	return []MenuItem{
		{ID: MenuBattleNormal, Label: "Battle (Normal)", Key: "1"},
		{ID: MenuBattleElite, Label: "Battle (Elite)", Key: "2"},
		{ID: MenuBattleBoss, Label: "Battle (BOSS)", Key: "3"},
		{ID: MenuShop, Label: "Shop", Key: "4"},
		{ID: MenuUpgrade, Label: "Upgrade Weapons", Key: "5"},
		{ID: MenuEquip, Label: "Equip Weapon", Key: "6"},
		{ID: MenuInventory, Label: "Inventory", Key: "7"},
		{ID: MenuStatus, Label: "Status", Key: "8"},
		{ID: MenuExit, Label: "Exit", Key: "9"},
	}
}
