#ifndef components
#define components

#include <array>
#include <atomic>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

static const std::vector<std::string> fonts = {
    "F0", "90", "90", "90", "F0",  // 0
    "20", "60", "20", "20", "70",  // 1
    "F0", "10", "F0", "80", "F0",  // 2
    "F0", "10", "F0", "10", "F0",  // 3
    "90", "90", "F0", "10", "10",  // 4
    "F0", "80", "F0", "10", "F0",  // 5
    "F0", "80", "F0", "90", "F0",  // 6
    "F0", "10", "20", "40", "40",  // 7
    "F0", "90", "F0", "90", "F0",  // 8
    "F0", "90", "F0", "10", "F0",  // 9
    "F0", "90", "F0", "90", "90",  // A
    "E0", "90", "E0", "90", "E0",  // B
    "F0", "80", "80", "80", "F0",  // C
    "E0", "90", "90", "90", "E0",  // D
    "F0", "80", "F0", "80", "F0",  // E
    "F0", "80", "F0", "80", "80"   // F
};

enum class Key {
  Zero,
  One,
  Two,
  Three,
  Four,
  Five,
  Six,
  Seven,
  Eight,
  Nine,
  A,
  B,
  C,
  D,
  E,
  F,
  Invalid
};

Key translateCharToKey(char key);
uint8_t translateKeyToChar(Key key);

std::string uint8ToHex(uint8_t value);

class Memory {
  std::vector<uint8_t> mem;
  uint16_t PC = 0;

 public:
  Memory();
  uint8_t getByte(const size_t index) const;
  uint8_t getByte(const std::string &index);
  void setByte(const size_t index, uint8_t value);
  void setByte(const size_t index, const std::string &value);
  void setByte(const std::string &index, const std::string &value);
  void setPC(uint16_t pc);
  uint16_t getPC();
  void print();
  void printInHex();
  void loadBinary(const std::string &directory);

 private:
  std::string findFirstBinaryFile(const std::string &directory,
                                  const std::vector<std::string> &extensions);
  std::vector<char> readBinaryFile(const std::string &filepath);
  void loadIntoMemory(const std::vector<char> &binary);
  size_t hexToIndex(const std::string &hexAddress) const;
  void loadFonts();
};

class Display {
  static const size_t rows = 32;
  static const size_t cols = 64;
  std::array<std::array<bool, cols>, rows> matrix = {};
  bool reprint = false;

 public:
  void setPixel(const size_t row, const size_t col, bool val);
  bool getPixel(const size_t row, const size_t col) const;
  void setAllPixels(bool val);
  void protoPrint();
  void setReprint(bool val);
  bool getReprint();
  void clear();
  size_t getRows() const;
  size_t getCols() const;
};

class Registers {
  std::vector<uint8_t> mem;

 public:
  Registers();
  Registers(size_t size);
  void setReg(size_t reg, uint8_t val);
  uint8_t getReg(size_t reg) const;
};

class Timer {
 public:
  Timer();
  void start(int interval_ms, const std::string &timer_name);
  uint8_t getValue() const;
  void setValue(uint8_t newValue);

 private:
  std::atomic<uint8_t> value;
  std::thread worker;
};

#endif  // components
