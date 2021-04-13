#include <iostream>
#include <stdexcept>

#include "game.h"

int main() {
  try {
    Game game;
    game.Run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << "\n";
  }

  return 0;
}
