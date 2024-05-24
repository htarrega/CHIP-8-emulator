#include <array>
#include <atomic>
#include <chrono>
#include <iostream>
#include <stack>
#include <string>
#include <thread>
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

// From 000 to 1FF
class Memory {
  std::vector<uint8_t> mem;

public:
  Memory() : mem(4096, 0) { loadFonts(); }

  auto begin() { return mem.begin(); }

  auto end() { return mem.end(); }

  uint8_t getByte(const size_t index) const {
    if (index < mem.size()) {
      return mem[index];
    }
    throw std::out_of_range("Index out of bounds");
  }

  uint8_t getByte(const std::string &index) {
    return getByte(hexToIndex(index));
  }

  void setByte(const size_t index, uint8_t value) {
    if (index < mem.size()) {
      mem[index] = value;
    } else {
      throw std::out_of_range("Index out of bounds");
    }
  }

  void setByte(const size_t index, const std::string &value) {
    setByte(index, hexToIndex(value));
  }

  void setByte(const std::string &index, const std::string &value) {
    setByte(hexToIndex(index), hexToIndex(value));
  }

  void print() {
    for (const auto byte : mem) {
      std::cout << unsigned(byte) << " ";
    }
    std::cout << std::endl;
  }

private:
  size_t hexToIndex(const std::string &hexAddress) const {
    return std::stoi(hexAddress, nullptr, 16);
  }
  void loadFonts() {
    size_t pos = 0;
    for (std::string byte : fonts) {
      setByte(pos, byte);
      pos++;
    }
  }
};

class Display {
  static const size_t rows = 64;
  static const size_t cols = 32;
  std::array<std::array<bool, rows>, cols> matrix = {};

public:
  void setPixel(const size_t row, const size_t col, bool val) {
    matrix[row][col] = val;
  }
  void protoPrint() {
    char printable;
    for (const auto &row : matrix) {
      for (bool valor : row) {
        if (valor) {
          printable = 'X';
        } else {
          printable = '.';
        }
        std::cout << printable << " ";
      }
      std::cout << std::endl;
    }
  }
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

int main(int argc, char **argv) {
  std::cout << "I'm EMU" << std::endl;
  // INITIALIZATIONS//
  Memory mem;
  Display disp;
  std::stack<u_int8_t> stack;

  int instructionsPerSecond = 700;
  std::chrono::milliseconds timeOrderInMs(1000);
  std::chrono::milliseconds timePerInstruction(timeOrderInMs.count() /
                                               instructionsPerSecond);

  //--------------//
  mem.setByte("00", "11");
  mem.setByte("1FF", "FF");
  mem.print();

  for (int i = 0; i <= 63; ++i) {
    disp.setPixel(1, i, true);
  }

  disp.protoPrint();

  while (true) {
    auto execStart = std::chrono::steady_clock::now();

    std::cout << "FETCH" << std::endl;
    std::cout << "DECODE" << std::endl;
    std::cout << "EXECUTE" << std::endl;

    auto execEnd = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        execEnd - execStart);

    if (elapsed < timePerInstruction) {
      auto remainingTime = timePerInstruction - elapsed;
      std::this_thread::sleep_for(remainingTime);
    }
  }
  return 0;
}
