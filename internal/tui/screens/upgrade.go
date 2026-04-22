// upgrade.go implements the weapon upgrade screen.
// Shows available upgrade recipes with material requirements, gold costs,
// and damage bonuses. Validates requirements before applying upgrades.
package screens

import (
	"fmt"
	"strings"

	"github.com/charmbracelet/bubbles/key"
	tea "github.com/charmbracelet/bubbletea"
	"github.com/tenyom/textrpg-tui/internal/domain"
	"github.com/tenyom/textrpg-tui/internal/infrastructure/registry"
	"github.com/tenyom/textrpg-tui/internal/tui/styles"
)

// Pre-allocated key bindings for the upgrade screen.
var (
	upgKeyUp    = key.NewBinding(key.WithKeys("up", "k"))
	upgKeyDown  = key.NewBinding(key.WithKeys("down", "j"))
	upgKeyEnter = key.NewBinding(key.WithKeys("enter"))
	upgKeyEsc   = key.NewBinding(key.WithKeys("esc", "backspace"))
)

// UpgradeMode represents the current upgrade submenu.
type UpgradeMode int

const (
	UpgradeModeMain UpgradeMode = iota
	UpgradeModeViewEquipped
	UpgradeModeViewInventory
	UpgradeModeDetails
)

// UpgradeScreen implements the weapon upgrade screen.
type UpgradeScreen struct {
	player   *domain.Player
	weapons  *registry.WeaponRegistry
	items    *registry.ItemRegistry
	upgrades *registry.UpgradeRegistry

	mode     UpgradeMode
	cursor   int
	width    int
	height   int
	message  string
	msgStyle string
}

// NewUpgradeScreen creates a new upgrade screen.
func NewUpgradeScreen(
	player *domain.Player,
	weapons *registry.WeaponRegistry,
	items *registry.ItemRegistry,
	upgrades *registry.UpgradeRegistry,
) *UpgradeScreen {
	return &UpgradeScreen{
		player:   player,
		weapons:  weapons,
		items:    items,
		upgrades: upgrades,
		mode:     UpgradeModeMain,
		cursor:   0,
	}
}

// Title returns the screen title.
func (u *UpgradeScreen) Title() string {
	return "WEAPON UPGRADE"
}

// SetSize updates dimensions.
func (u *UpgradeScreen) SetSize(width, height int) {
	u.width = width
	u.height = height
}

// Update handles upgrade screen input.
func (u *UpgradeScreen) Update(msg tea.Msg) tea.Cmd {
	switch msg := msg.(type) {
	case tea.KeyMsg:
		switch {
		case key.Matches(msg, upgKeyUp):
			if u.cursor > 0 {
				u.cursor--
			}
		case key.Matches(msg, upgKeyDown):
			u.cursor++
		case key.Matches(msg, upgKeyEnter):
			return u.handleSelect()
		case key.Matches(msg, upgKeyEsc):
			if u.mode != UpgradeModeMain {
				u.mode = UpgradeModeMain
				u.cursor = 0
				u.message = ""
			} else {
				return EndScreenCmd()
			}
		case msg.String() >= "1" && msg.String() <= "4":
			idx := int(msg.String()[0] - '1')
			u.cursor = idx
			return u.handleSelect()
		}
	}
	return nil
}

// handleSelect processes selection based on mode.
func (u *UpgradeScreen) handleSelect() tea.Cmd {
	switch u.mode {
	case UpgradeModeMain:
		switch u.cursor {
		case 0:
			u.doUpgradeEquipped()
		case 1:
			u.mode = UpgradeModeViewInventory
			u.cursor = 0
		case 2:
			u.mode = UpgradeModeDetails
			u.cursor = 0
		case 3: // Back
			return EndScreenCmd()
		}

	case UpgradeModeViewInventory:
		u.doUpgradeInventory()
	}
	return nil
}

// doUpgradeEquipped attempts to upgrade equipped weapon.
func (u *UpgradeScreen) doUpgradeEquipped() {
	if u.player.EquippedWeapon == nil {
		u.message = "No weapon equipped."
		u.msgStyle = "error"
		return
	}

	w := u.player.EquippedWeapon
	recipe := u.upgrades.GetRecipe(w.ID, w.Tier)

	if recipe == nil {
		if w.Tier >= w.MaxTier {
			u.message = "Weapon is at maximum tier!"
		} else {
			u.message = "No upgrade path available."
		}
		u.msgStyle = "error"
		return
	}

	// Check gold
	if u.player.Gold < recipe.GoldCost {
		u.message = fmt.Sprintf("Not enough gold! Need %d G", recipe.GoldCost)
		u.msgStyle = "error"
		return
	}

	// Check materials
	if !u.checkMaterials(recipe) {
		u.message = "Not enough materials!"
		u.msgStyle = "error"
		return
	}

	// Perform upgrade
	u.player.Gold -= recipe.GoldCost
	u.consumeMaterials(recipe)
	w.Tier = recipe.ToTier
	w.BaseDamage += recipe.DamageBonus

	u.message = fmt.Sprintf("Upgrade successful! %s +%d", w.Name, w.Tier-1)
	u.msgStyle = "success"
}

// doUpgradeInventory upgrades weapon from inventory.
func (u *UpgradeScreen) doUpgradeInventory() {
	upgradeables := u.getUpgradeableWeapons()
	if u.cursor < 0 || u.cursor >= len(upgradeables) {
		return
	}

	w := upgradeables[u.cursor].weapon
	recipe := u.upgrades.GetRecipe(w.ID, w.Tier)

	if recipe == nil {
		u.message = "No upgrade available."
		u.msgStyle = "error"
		return
	}

	if u.player.Gold < recipe.GoldCost {
		u.message = "Not enough gold!"
		u.msgStyle = "error"
		return
	}

	if !u.checkMaterials(recipe) {
		u.message = "Not enough materials!"
		u.msgStyle = "error"
		return
	}

	// Perform upgrade
	u.player.Gold -= recipe.GoldCost
	u.consumeMaterials(recipe)
	w.Tier = recipe.ToTier
	w.BaseDamage += recipe.DamageBonus

	u.message = fmt.Sprintf("Upgrade successful! %s +%d", w.Name, w.Tier-1)
	u.msgStyle = "success"
}

// checkMaterials checks if player has required materials.
func (u *UpgradeScreen) checkMaterials(recipe *registry.UpgradeRecipe) bool {
	for _, mat := range recipe.Materials {
		have := u.player.Inventory.CountItem(mat.ItemID)
		if have < mat.Quantity {
			return false
		}
	}
	return true
}

// consumeMaterials removes materials from inventory.
func (u *UpgradeScreen) consumeMaterials(recipe *registry.UpgradeRecipe) {
	for _, mat := range recipe.Materials {
		u.player.Inventory.RemoveItemByID(mat.ItemID, mat.Quantity)
	}
}

type upgradeableEntry struct {
	weapon   *domain.Weapon
	slotIdx  int
	equipped bool
}

// getUpgradeableWeapons returns weapons that can be upgraded.
func (u *UpgradeScreen) getUpgradeableWeapons() []upgradeableEntry {
	var result []upgradeableEntry

	// Check equipped
	if u.player.EquippedWeapon != nil && u.player.EquippedWeapon.CanUpgrade() {
		result = append(result, upgradeableEntry{
			weapon:   u.player.EquippedWeapon,
			slotIdx:  -1,
			equipped: true,
		})
	}

	// Check inventory
	for i := 0; i < domain.MaxInventorySlots; i++ {
		slot := u.player.Inventory.GetSlot(i)
		if slot != nil && slot.Type == domain.SlotWeapon {
			if slot.Weapon.CanUpgrade() {
				result = append(result, upgradeableEntry{
					weapon:   slot.Weapon,
					slotIdx:  i,
					equipped: false,
				})
			}
		}
	}

	return result
}

// View renders the upgrade screen.
// FORMAT MATCHES C src/game/upgrade.c upgrade_enter()
func (u *UpgradeScreen) View() string {
	var b strings.Builder

	// Header matching C: print_header("WEAPON UPGRADE", 50)
	b.WriteString(styles.SectionHeaderStyle.Render("=== WEAPON UPGRADE ==="))
	b.WriteString("\n\n")

	// Gold display
	goldLabel := styles.StatLabelStyle.Render("  Your Gold: ")
	goldValue := styles.GoldStyle.Render(fmt.Sprintf("%d G", u.player.Gold))
	b.WriteString(goldLabel + goldValue)
	b.WriteString("\n\n")

	// Content based on mode
	switch u.mode {
	case UpgradeModeMain:
		b.WriteString(u.renderUpgradeableWeapons())
		b.WriteString("\n")
		b.WriteString(u.renderMaterials())
		b.WriteString("\n")
		b.WriteString(u.renderMainMenu())
	case UpgradeModeViewInventory:
		b.WriteString(u.renderInventoryWeapons())
	case UpgradeModeDetails:
		b.WriteString(u.renderDetails())
	}

	// Status message
	if u.message != "" {
		b.WriteString("\n")
		switch u.msgStyle {
		case "success":
			b.WriteString(styles.SuccessStyle.Render(u.message))
		case "error":
			b.WriteString(styles.ErrorStyle.Render(u.message))
		default:
			b.WriteString(u.message)
		}
	}

	return b.String()
}

// renderUpgradeableWeapons matches C print_upgradeable_weapons().
func (u *UpgradeScreen) renderUpgradeableWeapons() string {
	var b strings.Builder

	b.WriteString(styles.SectionHeaderStyle.Render("=== Upgradeable Weapons ==="))
	b.WriteString("\n\n")

	upgradeables := u.getUpgradeableWeapons()

	if len(upgradeables) == 0 {
		b.WriteString(styles.DisabledItemStyle.Render("  No weapons can be upgraded."))
		b.WriteString("\n")
		b.WriteString(styles.DisabledItemStyle.Render("  (Collect materials from monsters or buy upgradeable weapons)"))
		return b.String()
	}

	for i, entry := range upgradeables {
		w := entry.weapon
		recipe := u.upgrades.GetRecipe(w.ID, w.Tier)

		var line strings.Builder
		if entry.equipped {
			line.WriteString("  [E] ")
		} else {
			line.WriteString(fmt.Sprintf("  %2d. ", i+1))
		}

		line.WriteString(w.DisplayName())
		if entry.equipped {
			line.WriteString(" (EQUIPPED)")
		}
		b.WriteString(styles.StatValueStyle.Render(line.String()))
		b.WriteString("\n")

		// Show upgrade path
		if recipe != nil {
			canUpgrade := u.player.Gold >= recipe.GoldCost && u.checkMaterials(recipe)
			status := "missing materials"
			if canUpgrade {
				status = "READY"
			}
			b.WriteString(fmt.Sprintf("      -> Tier %d (%s)\n", recipe.ToTier, status))
		}
	}

	return b.String()
}

// renderMaterials shows player's materials.
func (u *UpgradeScreen) renderMaterials() string {
	var b strings.Builder

	b.WriteString(styles.SectionHeaderStyle.Render("=== Materials ==="))
	b.WriteString("\n")

	materials := u.player.Inventory.GetMaterials()
	if len(materials) == 0 {
		b.WriteString(styles.DisabledItemStyle.Render("  No materials"))
		return b.String()
	}

	for _, mat := range materials {
		b.WriteString(fmt.Sprintf("  %s x%d\n", mat.Name, mat.Quantity))
	}

	return b.String()
}

// renderMainMenu matches C upgrade menu.
func (u *UpgradeScreen) renderMainMenu() string {
	var b strings.Builder

	b.WriteString(styles.SeparatorStyle.Render("--- Upgrade Menu ---"))
	b.WriteString("\n")

	items := []string{
		"1. Upgrade Equipped Weapon",
		"2. Upgrade Inventory Weapon",
		"3. View Details",
		"4. Back",
	}

	for i, item := range items {
		if i == u.cursor && u.mode == UpgradeModeMain {
			b.WriteString(styles.SelectedItemStyle.Render(item))
		} else {
			b.WriteString(styles.NormalItemStyle.Render(item))
		}
		b.WriteString("\n")
	}

	b.WriteString("Choice: ")
	return b.String()
}

// renderInventoryWeapons shows inventory weapons for upgrade selection.
func (u *UpgradeScreen) renderInventoryWeapons() string {
	var b strings.Builder

	b.WriteString(styles.SectionHeaderStyle.Render("=== Select Weapon to Upgrade ==="))
	b.WriteString("\n\n")

	upgradeables := u.getUpgradeableWeapons()
	if u.cursor >= len(upgradeables) && len(upgradeables) > 0 {
		u.cursor = len(upgradeables) - 1
	}

	for i, entry := range upgradeables {
		w := entry.weapon
		recipe := u.upgrades.GetRecipe(w.ID, w.Tier)

		var line strings.Builder
		line.WriteString(fmt.Sprintf("  %2d. %s", i+1, w.DisplayName()))
		if entry.equipped {
			line.WriteString(" (EQUIPPED)")
		}

		if recipe != nil {
			line.WriteString(fmt.Sprintf(" [%d G]", recipe.GoldCost))
		}

		if i == u.cursor {
			b.WriteString(styles.SelectedItemStyle.Render(line.String()))
		} else {
			b.WriteString(line.String())
		}
		b.WriteString("\n")

		// Show requirements for selected
		if i == u.cursor && recipe != nil {
			b.WriteString(u.renderRecipeRequirements(recipe))
		}
	}

	b.WriteString("\n")
	b.WriteString(styles.HelpDescStyle.Render("[Enter] Upgrade  [Esc] Back"))

	return b.String()
}

// renderRecipeRequirements shows materials needed for upgrade.
func (u *UpgradeScreen) renderRecipeRequirements(recipe *registry.UpgradeRecipe) string {
	var b strings.Builder

	b.WriteString("      Requirements:\n")

	// Gold
	goldOK := u.player.Gold >= recipe.GoldCost
	goldStatus := "[OK]"
	if !goldOK {
		goldStatus = fmt.Sprintf("[NEED %d more]", recipe.GoldCost-u.player.Gold)
	}
	b.WriteString(fmt.Sprintf("        Gold: %d %s\n", recipe.GoldCost, goldStatus))

	// Materials
	for _, mat := range recipe.Materials {
		have := u.player.Inventory.CountItem(mat.ItemID)
		need := mat.Quantity

		matName := mat.ItemID
		if item := u.items.GetByID(mat.ItemID); item != nil {
			matName = item.Name
		}

		status := "[OK]"
		if have < need {
			status = fmt.Sprintf("[NEED %d more]", need-have)
		}
		b.WriteString(fmt.Sprintf("        %s: %d/%d %s\n", matName, have, need, status))
	}

	return b.String()
}

// renderDetails shows detailed weapon info.
func (u *UpgradeScreen) renderDetails() string {
	var b strings.Builder

	b.WriteString(styles.SectionHeaderStyle.Render("=== Weapon Details ==="))
	b.WriteString("\n\n")

	if u.player.EquippedWeapon != nil {
		w := u.player.EquippedWeapon
		b.WriteString(fmt.Sprintf("  %s (EQUIPPED)\n", w.DisplayName()))
		b.WriteString(fmt.Sprintf("  Type: %s\n", w.Type.String()))
		b.WriteString(fmt.Sprintf("  Element: %s\n", w.Element.String()))
		b.WriteString(fmt.Sprintf("  Damage: %d\n", w.Damage()))
		b.WriteString(fmt.Sprintf("  Tier: %d/%d\n", w.Tier, w.MaxTier))

		recipe := u.upgrades.GetRecipe(w.ID, w.Tier)
		if recipe != nil {
			b.WriteString(fmt.Sprintf("  Next Tier Damage: +%d\n", recipe.DamageBonus))
		}
	} else {
		b.WriteString(styles.DisabledItemStyle.Render("  No weapon equipped"))
	}

	b.WriteString("\n")
	b.WriteString(styles.HelpDescStyle.Render("[Esc] Back"))

	return b.String()
}
