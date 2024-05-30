#ifndef components
#define components

#include <array>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

static const std::vector<std::string> fonts = {
    "F0", "90", "90", "90", "F0", // 0
    "20", "60", "20", "20", "70", // 1
    "F0", "10", "F0", "80", "F0", // 2
    "F0", "10", "F0", "10", "F0", // 3
    "90", "90", "F0", "10", "10", // 4
    "F0", "80", "F0", "10", "F0", // 5
    "F0", "80", "F0", "90", "F0", // 6
    "F0", "10", "20", "40", "40", // 7
    "F0", "90", "F0", "90", "F0", // 8
    "F0", "90", "F0", "10", "F0", // 9
    "F0", "90", "F0", "90", "90", // A
    "E0", "90", "E0", "90", "E0", // B
    "F0", "80", "80", "80", "F0", // C
    "E0", "90", "90", "90", "E0", // D
    "F0", "80", "F0", "80", "F0", // E
    "F0", "80", "F0", "80", "80"  // F
};

std::string uint8ToHex(uint8_t value);

class Memory {
  std::vector<uint8_t> mem;
  size_t PC = 0;

public:
  Memory();
  auto begin();
  auto end();
  uint8_t getByte(const size_t index) const;
  uint8_t getByte(const std::string &index);
  void setByte(const size_t index, uint8_t value);
  void setByte(const size_t index, const std::string &value);
  void setByte(const std::string &index, const std::string &value);
  void setPC(size_t pc);
  size_t getPC();
  void print();
  void printInHex();

private:
  size_t hexToIndex(const std::string &hexAddress) const;
  void loadFonts();
};

class Display {
  static const size_t rows = 64;
  static const size_t cols = 32;
  std::array<std::array<bool, rows>, cols> matrix = {};
  bool reprint = false;

public:
  void setPixel(const size_t row, const size_t col, bool val);
  void protoPrint();
  void setReprint(bool val);
  bool getReprint();
  void clear();
};

// class Timer {
//   uint8_t value = 255;

// public:
//   void decrementTimer() {
//     if (value == 0) {
//       value = 255;
//     } else {
//       value--;
//     }
//   }
//   uint8_t getValue() const { return value.load(); }

//   void runTimer(Timer &timer) {
//     while (true) {
//       std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
//       timer.decrementTimer();
//     }
//   }
// }

#endif // components
