package ui

import (
	"github.com/gdamore/tcell/v2"
)

// Key represents keyboard input types.
type Key int

const (
	KeyNone Key = iota
	KeyUp
	KeyDown
	KeyLeft
	KeyRight
	KeyEnter
	KeyEscape
	KeyBackspace
	KeyTab
	KeySpace
	KeyChar
	KeyCtrlC
	KeyCtrlQ
	KeyUnknown
)

// KeyEvent represents a keyboard event.
type KeyEvent struct {
	Key  Key
	Char rune
}

// String returns the key name.
func (k Key) String() string {
	names := map[Key]string{
		KeyNone:      "None",
		KeyUp:        "Up",
		KeyDown:      "Down",
		KeyLeft:      "Left",
		KeyRight:     "Right",
		KeyEnter:     "Enter",
		KeyEscape:    "Escape",
		KeyBackspace: "Backspace",
		KeyTab:       "Tab",
		KeySpace:     "Space",
		KeyChar:      "Char",
		KeyCtrlC:     "Ctrl+C",
		KeyCtrlQ:     "Ctrl+Q",
	}
	if name, ok := names[k]; ok {
		return name
	}
	return "Unknown"
}

// ConvertTcellKey converts tcell.EventKey to our KeyEvent.
func ConvertTcellKey(ev *tcell.EventKey) KeyEvent {
	switch ev.Key() {
	case tcell.KeyUp:
		return KeyEvent{Key: KeyUp}
	case tcell.KeyDown:
		return KeyEvent{Key: KeyDown}
	case tcell.KeyLeft:
		return KeyEvent{Key: KeyLeft}
	case tcell.KeyRight:
		return KeyEvent{Key: KeyRight}
	case tcell.KeyEnter:
		return KeyEvent{Key: KeyEnter}
	case tcell.KeyEscape:
		return KeyEvent{Key: KeyEscape}
	case tcell.KeyBackspace, tcell.KeyBackspace2:
		return KeyEvent{Key: KeyBackspace}
	case tcell.KeyTab:
		return KeyEvent{Key: KeyTab}
	case tcell.KeyCtrlC:
		return KeyEvent{Key: KeyCtrlC}
	case tcell.KeyCtrlQ:
		return KeyEvent{Key: KeyCtrlQ}
	case tcell.KeyRune:
		ch := ev.Rune()
		if ch == ' ' {
			return KeyEvent{Key: KeySpace}
		}
		return KeyEvent{Key: KeyChar, Char: ch}
	default:
		return KeyEvent{Key: KeyUnknown}
	}
}
