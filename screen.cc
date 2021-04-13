#include "screen.h"

#include <curses.h>

#include <stdexcept>

std::unique_ptr<Screen> Screen::Create() {
  if (setlocale(LC_ALL, "") == nullptr) {
    throw std::runtime_error("setlocale failed");
  }

  if (initscr() == nullptr) {
    throw std::runtime_error("initscr failed");
  }
  std::unique_ptr<Screen> ret(new Screen);

  timeout(0);
  if (raw() != OK) {
    throw std::runtime_error("raw failed");
  }
  if (noecho() != OK) {
    throw std::runtime_error("noecho failed");
  }
  if (keypad(stdscr, true) != OK) {
    throw std::runtime_error("keypad failed");
  }
  if (start_color() != OK) {
    throw std::runtime_error("start_color failed");
  }

  constexpr int kInvisible = 0;
  if (curs_set(kInvisible) == ERR) {
    throw std::runtime_error("curs_set failed");
  }

  constexpr int kEscapeDelayMs = 25;
  if (set_escdelay(kEscapeDelayMs) != OK) {
    throw std::runtime_error("set_escdelay failed");
  }

  ret->RefreshSize();
  ret->Refresh();

  return ret;
}

Screen::~Screen() { endwin(); }

void Screen::AddObserver(Observer* o) { observers_.insert(o); }
void Screen::RemoveObserver(Observer* o) { observers_.erase(o); }

void Screen::Clear() {
  if (clear() != OK) {
    throw std::runtime_error("clear failed");
  }
}

void Screen::Refresh() {
  if (refresh() != OK) {
    throw std::runtime_error("refresh failed");
  }
}

void Screen::SetChar(int row, int col, wchar_t val) {
  if (row < 0 || row >= rows_ || col < 0 || col >= cols_) {
    return;
  }

  if (mvaddch(row, col, val) != OK) {
    throw std::runtime_error("mvaddch failed");
  }
}

void Screen::SetChars(int row, int col, std::string_view str) {
  for (char c : str) {
    SetChar(row, col++, c);
  }
}

std::optional<Screen::KeyState> Screen::ReadKey() {
  while (true) {
    KeyState result;

    int res = get_wch(&result.code);
    switch (res) {
      case KEY_CODE_YES:
        result.is_key_code = true;
        break;
      case OK:
        break;
      default:
        return std::nullopt;
    }

    if (result.is_key_code && result.code == KEY_RESIZE) {
      RefreshSize();
      for (auto* obs : observers_) {
        obs->OnScreenSizeChanged();
      }
      continue;
    }

    constexpr wchar_t kEscape = 27;
    if (result.code == kEscape) {
      result.is_key_code = true;

      nodelay(stdscr, true);
      wint_t wch;
      res = get_wch(&wch);
      nodelay(stdscr, false);

      if (res == ERR) {
        throw std::runtime_error("mvaddch failed");
      }

      // If another key is available, then assume it is alt + key
      result.code = wch;
      result.alt = true;
    }

    return result;
  }
}

void Screen::RefreshSize() { getmaxyx(stdscr, rows_, cols_); }
