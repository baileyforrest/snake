#pragma once

#include <memory>
#include <set>
#include <string_view>

class Screen {
 public:
  struct KeyState {
    bool alt = false;
    bool ctrl = false;
    bool is_key_code = false;
    wint_t code = 0;
  };

  class Observer {
   public:
    ~Observer() = default;
    virtual void OnScreenSizeChanged() = 0;
  };

  static std::unique_ptr<Screen> Create();
  ~Screen();

  void AddObserver(Observer* o);
  void RemoveObserver(Observer* o);

  void Clear();
  void Refresh();
  void SetChar(int row, int col, wchar_t val);
  void SetChars(int row, int col, std::string_view str);

  std::optional<KeyState> ReadKey();

  int rows() const { return rows_; }
  int cols() const { return cols_; }

 private:
  Screen() = default;

  void RefreshSize();

  std::set<Observer*> observers_;

  int rows_ = 0;
  int cols_ = 0;
};
