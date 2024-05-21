#include <array>
#include <iostream>
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

// From 000 to 1FF
class memory {
  std::vector<uint8_t> mem;

public:
  memory() : mem(4096, 0) { void loadFonts(memory & mem); }

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
  void loadFonts(memory &mem, const std::vector<std::string> &fonts) {
    for (std::string byte : fonts) {
    }
  }
};

class display {
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

int main(int argc, char **argv) {
  std::cout << "I'm EMU" << std::endl;
  // INITIALIZATIONS//
  memory mem;
  display disp;
  //--------------//
  mem.setByte("00", "11");
  mem.setByte("1FF", "FF");
  mem.print();

  for (int i = 0; i <= 63; ++i) {
    disp.setPixel(1, i, true);
  }

  disp.protoPrint();
  return 0;
}
