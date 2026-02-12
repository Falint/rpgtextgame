package ui

import (
	"github.com/gdamore/tcell/v2"
)

// Color represents terminal colors.
type Color int

const (
	ColorDefault Color = iota
	ColorBlack
	ColorRed
	ColorGreen
	ColorYellow
	ColorBlue
	ColorMagenta
	ColorCyan
	ColorWhite
	ColorBrightBlack
	ColorBrightRed
	ColorBrightGreen
	ColorBrightYellow
	ColorBrightBlue
	ColorBrightMagenta
	ColorBrightCyan
	ColorBrightWhite
)

// toTcellColor converts our Color to tcell.Color.
func toTcellColor(c Color) tcell.Color {
	switch c {
	case ColorBlack:
		return tcell.ColorBlack
	case ColorRed:
		return tcell.ColorRed
	case ColorGreen:
		return tcell.ColorGreen
	case ColorYellow:
		return tcell.ColorYellow
	case ColorBlue:
		return tcell.ColorBlue
	case ColorMagenta:
		return tcell.ColorPurple
	case ColorCyan:
		return tcell.ColorTeal
	case ColorWhite:
		return tcell.ColorWhite
	case ColorBrightBlack:
		return tcell.ColorGray
	case ColorBrightRed:
		return tcell.ColorMaroon
	case ColorBrightGreen:
		return tcell.ColorLime
	case ColorBrightYellow:
		return tcell.ColorOlive
	case ColorBrightBlue:
		return tcell.ColorNavy
	case ColorBrightMagenta:
		return tcell.ColorFuchsia
	case ColorBrightCyan:
		return tcell.ColorAqua
	case ColorBrightWhite:
		return tcell.ColorSilver
	default:
		return tcell.ColorDefault
	}
}

// Renderer provides drawing primitives.
type Renderer struct {
	screen *Screen
	style  tcell.Style
}

// NewRenderer creates a renderer for the given screen.
func NewRenderer(screen *Screen) *Renderer {
	return &Renderer{
		screen: screen,
		style:  tcell.StyleDefault,
	}
}

// Reset resets to default style.
func (r *Renderer) Reset() {
	r.style = tcell.StyleDefault
}

// SetColor sets foreground and background colors.
func (r *Renderer) SetColor(fg, bg Color) {
	r.style = tcell.StyleDefault.
		Foreground(toTcellColor(fg)).
		Background(toTcellColor(bg))
}

// SetAttr sets text attributes.
func (r *Renderer) SetAttr(bold, underline, reverse bool) {
	r.style = r.style.Bold(bold).Underline(underline).Reverse(reverse)
}

// DrawText draws text at position.
func (r *Renderer) DrawText(x, y int, text string, style tcell.Style) {
	for i, ch := range text {
		r.screen.SetContent(x+i, y, ch, style)
	}
}

// DrawTextSimple draws text with current style.
func (r *Renderer) DrawTextSimple(x, y int, text string) {
	r.DrawText(x, y, text, r.style)
}

// DrawTextColor draws text with specified colors.
func (r *Renderer) DrawTextColor(x, y int, text string, fg, bg Color) {
	style := tcell.StyleDefault.
		Foreground(toTcellColor(fg)).
		Background(toTcellColor(bg))
	r.DrawText(x, y, text, style)
}

// DrawBox draws a box with Unicode borders.
func (r *Renderer) DrawBox(x, y, width, height int, title string, style tcell.Style) {
	if width < 2 || height < 2 {
		return
	}

	// Box drawing characters
	const (
		boxTL = '┌'
		boxTR = '┐'
		boxBL = '└'
		boxBR = '┘'
		boxH  = '─'
		boxV  = '│'
	)

	// Top border
	r.screen.SetContent(x, y, boxTL, style)
	for i := 1; i < width-1; i++ {
		r.screen.SetContent(x+i, y, boxH, style)
	}
	r.screen.SetContent(x+width-1, y, boxTR, style)

	// Title
	if title != "" && len(title) < width-4 {
		titleX := x + 2
		r.screen.SetContent(titleX-1, y, ' ', style)
		for i, ch := range title {
			r.screen.SetContent(titleX+i, y, ch, style)
		}
		r.screen.SetContent(titleX+len(title), y, ' ', style)
	}

	// Side borders
	for i := 1; i < height-1; i++ {
		r.screen.SetContent(x, y+i, boxV, style)
		r.screen.SetContent(x+width-1, y+i, boxV, style)
	}

	// Bottom border
	r.screen.SetContent(x, y+height-1, boxBL, style)
	for i := 1; i < width-1; i++ {
		r.screen.SetContent(x+i, y+height-1, boxH, style)
	}
	r.screen.SetContent(x+width-1, y+height-1, boxBR, style)
}

// DrawBoxSimple draws a box with default style.
func (r *Renderer) DrawBoxSimple(x, y, width, height int, title string) {
	r.DrawBox(x, y, width, height, title, tcell.StyleDefault)
}

// FillRect fills a rectangle with a character.
func (r *Renderer) FillRect(x, y, width, height int, ch rune, style tcell.Style) {
	for dy := 0; dy < height; dy++ {
		for dx := 0; dx < width; dx++ {
			r.screen.SetContent(x+dx, y+dy, ch, style)
		}
	}
}

// ClearRect clears a rectangular area.
func (r *Renderer) ClearRect(x, y, width, height int) {
	r.FillRect(x, y, width, height, ' ', tcell.StyleDefault)
}

// DrawHLine draws a horizontal line.
func (r *Renderer) DrawHLine(x, y, width int, ch rune, style tcell.Style) {
	for i := 0; i < width; i++ {
		r.screen.SetContent(x+i, y, ch, style)
	}
}

// DrawVLine draws a vertical line.
func (r *Renderer) DrawVLine(x, y, height int, ch rune, style tcell.Style) {
	for i := 0; i < height; i++ {
		r.screen.SetContent(x, y+i, ch, style)
	}
}
