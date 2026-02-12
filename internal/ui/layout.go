package ui

// LayoutConfig defines panel proportions.
type LayoutConfig struct {
	MenuPercent    float64
	ContentPercent float64
	ActionPercent  float64
}

// DefaultLayoutConfig returns standard 3-panel layout.
func DefaultLayoutConfig() LayoutConfig {
	return LayoutConfig{
		MenuPercent:    0.18,
		ContentPercent: 0.52,
		ActionPercent:  0.30,
	}
}

// Layout holds calculated panel dimensions.
type Layout struct {
	Width  int
	Height int

	// Panel X positions and widths
	MenuX        int
	MenuWidth    int
	ContentX     int
	ContentWidth int
	ActionX      int
	ActionWidth  int
}

// NewLayout calculates layout from screen size and config.
func NewLayout(width, height int, cfg LayoutConfig) Layout {
	l := Layout{
		Width:  width,
		Height: height,
	}

	// Calculate widths with minimum constraints
	minWidth := 10

	l.MenuWidth = int(float64(width) * cfg.MenuPercent)
	if l.MenuWidth < minWidth {
		l.MenuWidth = minWidth
	}

	l.ActionWidth = int(float64(width) * cfg.ActionPercent)
	if l.ActionWidth < minWidth {
		l.ActionWidth = minWidth
	}

	// Content gets remaining space
	l.ContentWidth = width - l.MenuWidth - l.ActionWidth
	if l.ContentWidth < minWidth {
		l.ContentWidth = minWidth
	}

	// Calculate X positions (0-indexed)
	l.MenuX = 0
	l.ContentX = l.MenuWidth
	l.ActionX = l.MenuWidth + l.ContentWidth

	return l
}

// Rect represents a rectangular region.
type Rect struct {
	X, Y          int
	Width, Height int
}

// MenuRect returns the menu panel rectangle.
func (l Layout) MenuRect() Rect {
	return Rect{X: l.MenuX, Y: 0, Width: l.MenuWidth, Height: l.Height}
}

// ContentRect returns the content panel rectangle.
func (l Layout) ContentRect() Rect {
	return Rect{X: l.ContentX, Y: 0, Width: l.ContentWidth, Height: l.Height}
}

// ActionRect returns the action panel rectangle.
func (l Layout) ActionRect() Rect {
	return Rect{X: l.ActionX, Y: 0, Width: l.ActionWidth, Height: l.Height}
}
