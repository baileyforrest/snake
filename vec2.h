#pragma once

struct Vec2 {
  int x = 0;
  int y = 0;

  Vec2& operator+=(const Vec2& rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
  }
};

inline bool operator==(const Vec2& lhs, const Vec2& rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator!=(const Vec2& lhs, const Vec2& rhs) {
  return !(lhs == rhs);
}

inline Vec2 operator+(const Vec2& lhs, const Vec2& rhs) {
  return {
      .x = lhs.x + rhs.x,
      .y = lhs.y + rhs.y,
  };
}
