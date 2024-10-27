#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include "components.hpp"

std::string uint8ToHex(uint8_t value) {
  static const char hexDigits[256][3] = {
      "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B",
      "0C", "0D", "0E", "0F", "10", "11", "12", "13", "14", "15", "16", "17",
      "18", "19", "1A", "1B", "1C", "1D", "1E", "1F", "20", "21", "22", "23",
      "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F",
      "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B",
      "3C", "3D", "3E", "3F", "40", "41", "42", "43", "44", "45", "46", "47",
      "48", "49", "4A", "4B", "4C", "4D", "4E", "4F", "50", "51", "52", "53",
      "54", "55", "56", "57", "58", "59", "5A", "5B", "5C", "5D", "5E", "5F",
      "60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6A", "6B",
      "6C", "6D", "6E", "6F", "70", "71", "72", "73", "74", "75", "76", "77",
      "78", "79", "7A", "7B", "7C", "7D", "7E", "7F", "80", "81", "82", "83",
      "84", "85", "86", "87", "88", "89", "8A", "8B", "8C", "8D", "8E", "8F",
      "90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9A", "9B",
      "9C", "9D", "9E", "9F", "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7",
      "A8", "A9", "AA", "AB", "AC", "AD", "AE", "AF", "B0", "B1", "B2", "B3",
      "B4", "B5", "B6", "B7", "B8", "B9", "BA", "BB", "BC", "BD", "BE", "BF",
      "C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "CA", "CB",
      "CC", "CD", "CE", "CF", "D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7",
      "D8", "D9", "DA", "DB", "DC", "DD", "DE", "DF", "E0", "E1", "E2", "E3",
      "E4", "E5", "E6", "E7", "E8", "E9", "EA", "EB", "EC", "ED", "EE", "EF",
      "F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "FA", "FB",
      "FC", "FD", "FE", "FF"};

  return hexDigits[value];
}

Key translateCharToKey(char key) {
  switch (key) {
  case '0':
    return Key::Zero;
  case '1':
    return Key::One;
  case '2':
    return Key::Two;
  case '3':
    return Key::Three;
  case '4':
    return Key::Four;
  case '5':
    return Key::Five;
  case '6':
    return Key::Six;
  case '7':
    return Key::Seven;
  case '8':
    return Key::Eight;
  case '9':
    return Key::Nine;
  case 'A':
    return Key::A;
  case 'B':
    return Key::B;
  case 'C':
    return Key::C;
  case 'D':
    return Key::D;
  case 'E':
    return Key::E;
  case 'F':
    return Key::F;
  case 'G':
    return Key::G;
  default:
    return Key::Invalid;
  }
}

char translateKeyToChar(Key key) {
  switch (key) {
  case Key::Zero:
    return '0';
  case Key::One:
    return '1';
  case Key::Two:
    return '2';
  case Key::Three:
    return '3';
  case Key::Four:
    return '4';
  case Key::Five:
    return '5';
  case Key::Six:
    return '6';
  case Key::Seven:
    return '7';
  case Key::Eight:
    return '8';
  case Key::Nine:
    return '9';
  case Key::A:
    return 'A';
  case Key::B:
    return 'B';
  case Key::C:
    return 'C';
  case Key::D:
    return 'D';
  case Key::E:
    return 'E';
  case Key::F:
    return 'F';
  case Key::G:
    return 'G';
  default:
    return '\0'; // Returning null character for invalid keys
  }
}

// From 000 to 1FF
Memory::Memory() : mem(4096, 0) { loadFonts(); }

auto Memory::begin() { return mem.begin(); }

auto Memory::end() { return mem.end(); }

uint8_t Memory::getByte(const size_t index) const {
  if (index < mem.size()) {
    return mem[index];
  }
  throw std::out_of_range("Index out of bounds");
}

uint8_t Memory::getByte(const std::string &index) {
  return getByte(hexToIndex(index));
}

void Memory::setByte(const size_t index, uint8_t value) {
  if (index < mem.size()) {
    mem[index] = value;
  } else {
    throw std::out_of_range("Index out of bounds");
  }
}

void Memory::setByte(const size_t index, const std::string &value) {
  setByte(index, hexToIndex(value));
}

void Memory::setByte(const std::string &index, const std::string &value) {
  setByte(hexToIndex(index), hexToIndex(value));
}

void Memory::setPC(uint16_t pc) { PC = pc; }

uint16_t Memory::getPC() { return PC; }

void Memory::print() {
  for (const auto byte : mem) {
    std::cout << unsigned(byte) << " ";
  }
  std::cout << std::endl;
}

void Memory::printInHex() {
  for (const auto byte : mem) {
    std::string hexValue = uint8ToHex(byte);
    std::cout << hexValue << " ";
  }
  std::cout << std::endl;
}

size_t Memory::hexToIndex(const std::string &hexAddress) const {
  return std::stoi(hexAddress, nullptr, 16);
}

void Memory::loadFonts() {
  size_t pos = 80;
  for (const std::string &byte : fonts) {
    setByte(pos, byte);
    pos++;
  }
}

void Memory::loadBinary(const std::string &directory) {
  const std::vector<std::string> extensions = {".ch8"};
  std::string filepath = findFirstBinaryFile(directory, extensions);

  if (filepath.empty()) {
    std::cerr << "No binary file with extension .ch8 found in directory: "
              << directory << std::endl;
    exit(1);
  }

  std::vector<char> binary = readBinaryFile(filepath);
  loadIntoMemory(binary);
}

std::string
Memory::findFirstBinaryFile(const std::string &directory,
                            const std::vector<std::string> &extensions) {

  for (const auto &entry : std::filesystem::directory_iterator(directory)) {
    if (!entry.is_regular_file())
      continue;

    std::string extension = entry.path().extension().string();
    if (std::find(extensions.begin(), extensions.end(), extension) !=
        extensions.end()) {
      return entry.path().string();
    }
  }

  return "";
}

std::vector<char> Memory::readBinaryFile(const std::string &filepath) {
  std::ifstream ifs(filepath, std::ios::binary | std::ios::ate);
  if (!ifs) {
    std::cerr << "Error opening file: " << filepath << std::endl;
    exit(1);
  }

  std::ifstream::pos_type pos = ifs.tellg();
  std::vector<char> binary(pos);
  ifs.seekg(0, std::ios::beg);
  ifs.read(&binary[0], pos);

  if (!ifs) {
    std::cerr << "Error reading file: " << filepath << std::endl;
    exit(1);
  }

  return binary;
}

void Memory::loadIntoMemory(const std::vector<char> &binary) {
  auto pc = getPC();
  for (auto byte : binary) {
    setByte(pc, byte);
    pc++;
  }
}

void Display::setPixel(const size_t row, const size_t col, bool val) {
  matrix[row][col] = val;
}

bool Display::getPixel(const size_t row, const size_t col) const {
  return matrix[row][col];
}

void Display::protoPrint() {
  for (const auto &row : matrix) {
    for (bool valor : row) {
      std::cout << (valor ? "█" : " ");
    }
    std::cout << std::endl;
  }
}

void Display::setAllPixels(bool val) {
  for (auto &row : matrix) {
    for (bool &pixel : row) {
      pixel = val;
    }
  }
}

void Display::setReprint(bool val) { reprint = val; }

bool Display::getReprint() { return reprint; }

void Display::clear() {
  char printable;
  for (const auto &row : matrix) {
    for (bool valor : row) {
      valor = false;
    }
  }
  setReprint(true);
}

size_t Display::getCols() const { return cols; }

size_t Display::getRows() const { return rows; }

Registers::Registers() : mem(16, 0) {}

Registers::Registers(size_t size) : mem(size, 0) {}

void Registers::setReg(size_t reg, uint8_t val) {
  if (reg >= mem.size() || reg < 0) {
    throw std::out_of_range("Register index out of range");
  }
  mem[reg] = val & 0xFF;
}

uint8_t Registers::getReg(size_t reg) const {
  if (reg >= mem.size() || reg < 0) {
    throw std::out_of_range("Register index out of range");
  }
  return mem[reg];
}
Timer::Timer() : value(255) {}

void Timer::start(int interval_ms, const std::string &timer_name) {
  worker = std::thread([this, interval_ms, timer_name]() {
    while (true) {
      std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
      if (value > 0) {
        value--;
      }
    }
  });
  worker.detach();
}
uint8_t Timer::getValue() const { return value.load(); }
void Timer::setValue(uint8_t newValue) { value.store(newValue); }