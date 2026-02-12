// Package ui provides the Terminal User Interface.
//
// This package uses tcell for terminal control (Pure Go, no CGO).
// Responsibilities:
// - Screen initialization and cleanup
// - Keyboard input handling
// - Rendering with colors and box drawing
// - 3-panel layout management
//
// Dependencies: github.com/gdamore/tcell/v2
package ui

import (
	"github.com/gdamore/tcell/v2"
)

// Screen wraps tcell.Screen with our interface.
// All terminal operations go through this abstraction.
type Screen struct {
	screen tcell.Screen
	width  int
	height int
}

// NewScreen creates and initializes a new screen.
func NewScreen() (*Screen, error) {
	s, err := tcell.NewScreen()
	if err != nil {
		return nil, err
	}

	if err := s.Init(); err != nil {
		return nil, err
	}

	s.EnableMouse()
	s.EnablePaste()
	s.Clear()

	w, h := s.Size()
	return &Screen{
		screen: s,
		width:  w,
		height: h,
	}, nil
}

// Close cleans up the screen and restores terminal.
func (s *Screen) Close() {
	if s.screen != nil {
		s.screen.Fini()
	}
}

// Clear clears the entire screen.
func (s *Screen) Clear() {
	s.screen.Clear()
}

// Show synchronizes the screen buffer to terminal.
func (s *Screen) Show() {
	s.screen.Show()
}

// Sync forces a full redraw.
func (s *Screen) Sync() {
	s.screen.Sync()
}

// Size returns current terminal dimensions.
func (s *Screen) Size() (width, height int) {
	return s.screen.Size()
}

// UpdateSize refreshes stored dimensions.
func (s *Screen) UpdateSize() {
	s.width, s.height = s.screen.Size()
}

// Width returns terminal width.
func (s *Screen) Width() int {
	return s.width
}

// Height returns terminal height.
func (s *Screen) Height() int {
	return s.height
}

// SetContent sets a single cell.
func (s *Screen) SetContent(x, y int, ch rune, style tcell.Style) {
	s.screen.SetContent(x, y, ch, nil, style)
}

// PollEvent waits for the next event (key, resize, etc.).
func (s *Screen) PollEvent() tcell.Event {
	return s.screen.PollEvent()
}

// HideCursor hides the terminal cursor.
func (s *Screen) HideCursor() {
	s.screen.HideCursor()
}

// ShowCursor shows the cursor at position.
func (s *Screen) ShowCursor(x, y int) {
	s.screen.ShowCursor(x, y)
}
