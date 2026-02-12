// Package styles provides Lipgloss styles for the TextRPG TUI.
// Following spiketrace pattern for consistency.
package styles

import "github.com/charmbracelet/lipgloss"

// -----------------------------------------------------------------------------
// Color Palette (RPG Theme)
// -----------------------------------------------------------------------------

var (
	// Primary colors
	ColorPrimary   = lipgloss.Color("62")  // Soft blue (menu selection)
	ColorSecondary = lipgloss.Color("241") // Gray (borders)
	ColorAccent    = lipgloss.Color("205") // Pink/magenta (titles)
	ColorGold      = lipgloss.Color("220") // Gold (currency)
	ColorHP        = lipgloss.Color("196") // Red (health)
	ColorMana      = lipgloss.Color("39")  // Cyan (mana/mp)

	// Status colors
	ColorSuccess = lipgloss.Color("42")  // Green (heal, win)
	ColorWarning = lipgloss.Color("214") // Orange (low HP)
	ColorDanger  = lipgloss.Color("196") // Red (damage, critical)
	ColorMuted   = lipgloss.Color("240") // Dark gray

	// Element colors
	ColorFire  = lipgloss.Color("202") // Orange-red
	ColorWater = lipgloss.Color("39")  // Cyan
	ColorEarth = lipgloss.Color("136") // Brown/tan
	ColorWind  = lipgloss.Color("48")  // Light green

	// Text colors
	ColorText     = lipgloss.Color("252") // Light gray
	ColorTextDim  = lipgloss.Color("245") // Dimmed text
	ColorTextBold = lipgloss.Color("255") // White
)

// -----------------------------------------------------------------------------
// Panel Styles (3-Block Layout)
// -----------------------------------------------------------------------------

var (
	// BasePanelStyle is the foundation style for all panels.
	BasePanelStyle = lipgloss.NewStyle().
			Border(lipgloss.RoundedBorder()).
			BorderForeground(ColorSecondary).
			Padding(0, 1)

	// ActivePanelStyle is used for the currently focused panel.
	ActivePanelStyle = BasePanelStyle.
				BorderForeground(ColorPrimary)

	// PanelTitleStyle styles panel titles.
	PanelTitleStyle = lipgloss.NewStyle().
			Bold(true).
			Foreground(ColorAccent).
			Padding(0, 1)
)

// -----------------------------------------------------------------------------
// Menu Item Styles (Block 1)
// -----------------------------------------------------------------------------

var (
	// SelectedItemStyle is for the currently selected menu item.
	SelectedItemStyle = lipgloss.NewStyle().
				Foreground(ColorTextBold).
				Background(ColorPrimary).
				Bold(true).
				Padding(0, 1)

	// NormalItemStyle is for unselected menu items.
	NormalItemStyle = lipgloss.NewStyle().
			Foreground(ColorText).
			Padding(0, 1)

	// DisabledItemStyle is for unavailable options.
	DisabledItemStyle = lipgloss.NewStyle().
				Foreground(ColorMuted).
				Padding(0, 1)
)

// -----------------------------------------------------------------------------
// Character Stats Styles (Block 2)
// -----------------------------------------------------------------------------

var (
	// StatLabelStyle is for stat labels (HP:, ATK:, etc).
	StatLabelStyle = lipgloss.NewStyle().
			Foreground(ColorTextDim)

	// StatValueStyle is for stat values.
	StatValueStyle = lipgloss.NewStyle().
			Foreground(ColorText)

	// StatHighlightStyle is for important values.
	StatHighlightStyle = lipgloss.NewStyle().
				Foreground(ColorWarning).
				Bold(true)

	// GoldStyle is for gold display.
	GoldStyle = lipgloss.NewStyle().
			Foreground(ColorGold).
			Bold(true)

	// HPBarStyle is the base HP bar style.
	HPFullStyle = lipgloss.NewStyle().
			Foreground(ColorSuccess)

	// HPLowStyle is for low HP (< 30%).
	HPLowStyle = lipgloss.NewStyle().
			Foreground(ColorDanger).
			Bold(true)
)

// -----------------------------------------------------------------------------
// Combat Styles (Block 3 - Battle)
// -----------------------------------------------------------------------------

var (
	// DamageStyle is for damage numbers.
	DamageStyle = lipgloss.NewStyle().
			Foreground(ColorDanger).
			Bold(true)

	// HealStyle is for healing numbers.
	HealStyle = lipgloss.NewStyle().
			Foreground(ColorSuccess).
			Bold(true)

	// CritStyle is for critical hits.
	CritStyle = lipgloss.NewStyle().
			Foreground(ColorWarning).
			Bold(true).
			Italic(true)

	// BuffStyle is for buff notifications.
	BuffStyle = lipgloss.NewStyle().
			Foreground(ColorPrimary)

	// EnemyNameStyle is for enemy names.
	EnemyNameStyle = lipgloss.NewStyle().
			Foreground(ColorDanger).
			Bold(true)
)

// -----------------------------------------------------------------------------
// Shop Styles (Block 3 - Shop)
// -----------------------------------------------------------------------------

var (
	// PriceStyle is for item prices.
	PriceStyle = lipgloss.NewStyle().
			Foreground(ColorGold)

	// SoldOutStyle is for sold out items.
	SoldOutStyle = lipgloss.NewStyle().
			Foreground(ColorMuted).
			Strikethrough(true)

	// AffordableStyle is for items player can buy.
	AffordableStyle = lipgloss.NewStyle().
			Foreground(ColorSuccess)

	// NotAffordableStyle is for items player cannot buy.
	NotAffordableStyle = lipgloss.NewStyle().
				Foreground(ColorDanger)
)

// -----------------------------------------------------------------------------
// Element Styles
// -----------------------------------------------------------------------------

var (
	FireStyle = lipgloss.NewStyle().
			Foreground(ColorFire).
			Bold(true)

	WaterStyle = lipgloss.NewStyle().
			Foreground(ColorWater).
			Bold(true)

	EarthStyle = lipgloss.NewStyle().
			Foreground(ColorEarth).
			Bold(true)

	WindStyle = lipgloss.NewStyle().
			Foreground(ColorWind).
			Bold(true)
)

// -----------------------------------------------------------------------------
// Status Bar Styles (Footer)
// -----------------------------------------------------------------------------

var (
	// StatusBarStyle is the main status bar style.
	StatusBarStyle = lipgloss.NewStyle().
			Foreground(ColorTextDim).
			Background(lipgloss.Color("236")).
			Padding(0, 1)

	// HelpKeyStyle is for keyboard shortcut keys.
	HelpKeyStyle = lipgloss.NewStyle().
			Foreground(ColorPrimary).
			Bold(true)

	// HelpDescStyle is for keyboard shortcut descriptions.
	HelpDescStyle = lipgloss.NewStyle().
			Foreground(ColorTextDim)
)

// -----------------------------------------------------------------------------
// Loading & Error Styles
// -----------------------------------------------------------------------------

var (
	// LoadingStyle is for loading indicators.
	LoadingStyle = lipgloss.NewStyle().
			Foreground(ColorPrimary).
			Italic(true)

	// ErrorStyle is for error messages.
	ErrorStyle = lipgloss.NewStyle().
			Foreground(ColorDanger).
			Bold(true)

	// SuccessStyle is for success messages.
	SuccessStyle = lipgloss.NewStyle().
			Foreground(ColorSuccess).
			Bold(true)
)

// -----------------------------------------------------------------------------
// Section Styles
// -----------------------------------------------------------------------------

var (
	// SectionHeaderStyle for section headers like "=== WEAPONS ==="
	SectionHeaderStyle = lipgloss.NewStyle().
				Foreground(ColorAccent).
				Bold(true)

	// SeparatorStyle for separator lines.
	SeparatorStyle = lipgloss.NewStyle().
			Foreground(ColorSecondary)

	// DimItemStyle is for less important items.
	DimItemStyle = lipgloss.NewStyle().
			Foreground(ColorTextDim).
			Padding(0, 1) // Match menu item padding

	// CategoryMaterial is for item categories or special tags.
	CategoryMaterial = lipgloss.NewStyle().
				Foreground(lipgloss.Color("172")) // Orange-gold
)
