#include "terminal.hpp"
#include <cstdio>
#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <conio.h>
// Defined in newer SDKs; guard for older MinGW
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

static DWORD origConsoleMode = 0;
static HANDLE hStdin  = INVALID_HANDLE_VALUE;
static HANDLE hStdout = INVALID_HANDLE_VALUE;

void Terminal::enableANSI() {
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    hStdin  = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hStdout, &mode);
    SetConsoleMode(hStdout, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    // UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

void Terminal::enableRawMode() {
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hStdin, &origConsoleMode);
    DWORD rawMode = origConsoleMode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);
    SetConsoleMode(hStdin, rawMode);
}

void Terminal::disableRawMode() {
    if (hStdin != INVALID_HANDLE_VALUE)
        SetConsoleMode(hStdin, origConsoleMode);
}

void Terminal::getSize(int& cols, int& rows) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        cols = csbi.srWindow.Right  - csbi.srWindow.Left + 1;
        rows = csbi.srWindow.Bottom - csbi.srWindow.Top  + 1;
    } else {
        cols = 80; rows = 24;
    }
}

Key Terminal::readKey(char& outChar) {
    outChar = 0;
    int ch = _getch();
    
    // EOF or error
    if (ch == EOF) {
        Sleep(50);
        return Key::Unknown;
    }

    // Extended keys (arrows)
    if (ch == 0 || ch == 224) {
        int ext = _getch();
        switch (ext) {
            case 72: return Key::Up;
            case 80: return Key::Down;
            case 75: return Key::Left;
            case 77: return Key::Right;
        }
        return Key::Unknown;
    }

    // Standard keys
    if (ch == 3)  return Key::CtrlC;
    if (ch == 17) return Key::CtrlQ;
    if (ch == 13) return Key::Enter;
    if (ch == 27) return Key::Escape;
    if (ch == 8)  return Key::Backspace;

    if (ch >= '1' && ch <= '9') {
        outChar = static_cast<char>(ch);
        return static_cast<Key>(static_cast<int>(Key::K1) + (ch - '1'));
    }
    if (ch >= 32 && ch < 127) { 
        outChar = static_cast<char>(ch); 
        return Key::Char; 
    }
    return Key::Unknown;
}

#else
// POSIX fallback
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

static struct termios origTermios;

void Terminal::enableANSI() {}

void Terminal::enableRawMode() {
    tcgetattr(STDIN_FILENO, &origTermios);
    struct termios raw = origTermios;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    raw.c_cc[VMIN]  = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void Terminal::disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &origTermios);
}

void Terminal::getSize(int& cols, int& rows) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        cols = ws.ws_col; rows = ws.ws_row;
    } else { cols = 80; rows = 24; }
}

Key Terminal::readKey(char& outChar) {
    outChar = 0;
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1) return Key::Unknown;
    if (c == 3)  return Key::CtrlC;
    if (c == 17) return Key::CtrlQ;
    if (c == '\n' || c == '\r') return Key::Enter;
    if (c == 27) {
        char seq[3];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return Key::Escape;
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return Key::Escape;
        if (seq[0] == '[') {
            switch (seq[1]) {
            case 'A': return Key::Up;
            case 'B': return Key::Down;
            case 'C': return Key::Right;
            case 'D': return Key::Left;
            }
        }
        return Key::Escape;
    }
    if (c == 127) return Key::Backspace;
    if (c >= '1' && c <= '9') {
        outChar = c;
        return static_cast<Key>(static_cast<int>(Key::K1) + (c - '1'));
    }
    if (c >= 32 && c < 127) { outChar = c; return Key::Char; }
    return Key::Unknown;
}
#endif

void Terminal::flush() { fflush(stdout); }
