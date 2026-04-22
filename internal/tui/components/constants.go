// Package components provides reusable TUI components.
// Component constants define menu identifiers matching the C main.c print_main_menu() options.
package components

// -----------------------------------------------------------------------------
// Menu Item IDs (matching C main.c print_main_menu)
// -----------------------------------------------------------------------------

// Menu item ID constants for each main menu option.
// These string IDs decouple menu rendering from action dispatching,
// allowing handleMenuSelect() to switch on IDs without positional coupling.
const (
	MenuBattleNormal = "battle_normal" // Start combat vs Normal-tier monster
	MenuBattleElite  = "battle_elite"  // Start combat vs Elite-tier monster
	MenuBattleBoss   = "battle_boss"   // Start combat vs Boss-tier monster
	MenuShop         = "shop"          // Open the shop screen (buy/sell)
	MenuUpgrade      = "upgrade"       // Open weapon upgrade screen
	MenuEquip        = "equip"         // Open weapon equip screen
	MenuInventory    = "inventory"     // Open inventory screen
	MenuStatus       = "status"        // Open player status screen
	MenuExit         = "exit"          // Quit the application
)

// MenuItem represents a single main menu option.
// Contains everything needed for both display and action handling.
type MenuItem struct {
	ID    string // Action identifier used by handleMenuSelect() for dispatching
	Label string // Display text shown next to the number key
	Key   string // Display key character ("1" through "9")
}

// MainMenuItems returns all 9 menu items in display order.
// Order and labels match C main.c print_main_menu() exactly.
func MainMenuItems() []MenuItem {
	return []MenuItem{
		{ID: MenuBattleNormal, Label: "Battle (Normal)", Key: "1"}, // Normal encounter
		{ID: MenuBattleElite, Label: "Battle (Elite)", Key: "2"},   // Elite encounter
		{ID: MenuBattleBoss, Label: "Battle (BOSS)", Key: "3"},     // Boss encounter
		{ID: MenuShop, Label: "Shop", Key: "4"},                    // Buy/sell items
		{ID: MenuUpgrade, Label: "Upgrade Weapons", Key: "5"},      // Weapon tier upgrades
		{ID: MenuEquip, Label: "Equip Weapon", Key: "6"},           // Change equipped weapon
		{ID: MenuInventory, Label: "Inventory", Key: "7"},          // View/use items
		{ID: MenuStatus, Label: "Status", Key: "8"},                // View player stats
		{ID: MenuExit, Label: "Exit", Key: "9"},                    // Quit game
	}
}
