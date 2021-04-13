#pragma once

#include <deque>
#include <vector>

#include "screen.h"
#include "vec2.h"

class Game : public Screen::Observer {
 public:
  Game();
  ~Game() = default;

  void Run();

  // Screen::Observer implementation:
  void OnScreenSizeChanged() override { Reset(); }

 private:
  static constexpr int kInitialDelay = 100000;

  enum class State {
    kPlaying,
    kGameOver,
  };

  Vec2 RandomVec2();
  void DrawPixel(const Vec2& vec, int val);
  void Reset();
  bool RunIter();

  std::unique_ptr<Screen> screen_{Screen::Create()};
  unsigned rand_seed_ = 0;
  State state_ = State::kPlaying;

  int rows_ = 0;
  int cols_ = 0;

  std::deque<Vec2> snake_;
  Vec2 apple_;
  std::optional<Vec2> dir_;
  int score_ = 0;
  int delay_us_ = kInitialDelay;
};
