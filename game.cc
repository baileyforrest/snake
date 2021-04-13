#include "game.h"

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <sstream>
#include <stdexcept>

Game::Game() {
  rand_seed_ = time(nullptr);
  screen_->AddObserver(this);
  Reset();
}

void Game::Run() {
  bool game_over = false;

  while (true) {
    std::optional<Screen::KeyState> key_state = screen_->ReadKey();
    if (key_state.has_value()) {
      switch (key_state->code) {
        case KEY_UP:
          dir_ = {.x = 0, .y = -1};
          break;
        case KEY_DOWN:
          dir_ = {.x = 0, .y = 1};
          break;
        case KEY_LEFT:
          dir_ = {.x = -1, .y = 0};
          break;
        case KEY_RIGHT:
          dir_ = {.x = 1, .y = 0};
          break;
        case 'q':
          return;
      }

      game_over = false;
    }

    if (!game_over && RunIter()) {
      game_over = true;

      auto print_center_message = [&](int row, std::string_view message) {
        int x = std::max<int>(0, screen_->cols() / 2 - message.size() / 2);
        screen_->SetChars(row, /*col=*/x, message);
      };

      std::ostringstream message;
      message << "Game over! Score: " << score_;
      print_center_message(screen_->rows() / 2, message.str());
      print_center_message(screen_->rows() / 2 + 1,
                           "Press q to quit, or an arrow to play again");

      Reset();
      screen_->Refresh();
    }

    usleep(delay_us_);
  }
}

Vec2 Game::RandomVec2() {
  return {
      .x = 1 + rand_r(&rand_seed_) % (cols_ - 2),
      .y = 1 + rand_r(&rand_seed_) % (rows_ - 2),
  };
}

void Game::DrawPixel(const Vec2& vec, int val) {
  int x = vec.x * 2;

  screen_->SetChar(vec.y, x, val);
  screen_->SetChar(vec.y, x + 1, val);
}

void Game::Reset() {
  rows_ = screen_->rows();
  if (rows_ < 4) {
    throw std::runtime_error("rows too small");
  }

  cols_ = screen_->cols() / 2;
  if (cols_ < 4) {
    throw std::runtime_error("cols too small");
  }

  snake_.clear();
  snake_.push_back({.x = cols_ / 2, .y = rows_ / 2});
  apple_ = RandomVec2();

  dir_ = std::nullopt;
  score_ = 0;
  delay_us_ = kInitialDelay;
}

bool Game::RunIter() {
  Vec2 head = snake_.front();
  if (dir_.has_value()) {
    head += *dir_;

    // Crashed into wall.
    if (head.x < 1 || head.x >= cols_ - 1 || head.y < 1 ||
        head.y >= rows_ - 1) {
      return true;
    }

    // Crashed into self.
    for (const auto& seg : snake_) {
      if (head == seg) {
        return true;
      }
    }

    snake_.push_front(head);
    if (head == apple_) {
      apple_ = RandomVec2();
      ++score_;
      delay_us_ = std::max(delay_us_ - 1000, 0);
    } else {
      snake_.pop_back();
    }
  }

  // Draw the board.
  {
    screen_->Clear();

    // Draw the border.
    for (int x = 0; x < cols_; ++x) {
      DrawPixel({.x = x, .y = 0}, 'X');
      DrawPixel({.x = x, .y = rows_ - 1}, 'X');
    }

    for (int y = 0; y < rows_; ++y) {
      DrawPixel({.x = 0, .y = y}, 'X');
      DrawPixel({.x = cols_ - 1, .y = y}, 'X');
    }

    std::ostringstream score;
    score << "Score: " << score_ << " ";

    screen_->SetChars(/*row=*/0, /*col=*/0, score.str());

    DrawPixel(apple_, 'A');

    for (const auto& seg : snake_) {
      DrawPixel(seg, 'S');
    }

    screen_->Refresh();
  }

  return false;
}
