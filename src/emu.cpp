#include <array>
#include <atomic>
#include <chrono>
#include <iostream>
#include <stack>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "components/components.hpp"

uint16_t fetch(components::Memory &mem) {
  size_t pc = mem.getPC();
  uint16_t firstHalf = mem.getByte(pc) << 8;
  uint16_t secondHalf = mem.getByte(pc + 1);
  mem.setPC(pc + 2);
  return firstHalf | secondHalf;
}

bool decodeAndExecute(uint16_t instruction) {
  uint8_t instCode = (instCode >> 4) & 0x0F;
  switch (instCode) {
  case 0x0:
    break;
  case 0x1:
    break;
  case 0x6:
    break;
  case 0x7:
    break;
  case 0xA:
    break;
  case 0xD:
    break;
  default:
    std::cout << "Opcode does not exist." << std::endl;
    exit(0);
  }
}

int main(int argc, char **argv) {
  std::cout << "I'm EMU" << std::endl;
  // INITIALIZATIONS//
  components::Memory mem;
  components::Display disp;
  std::stack<u_int8_t> stack;

  int instructionsPerSecond = 700;
  std::chrono::milliseconds timeOrderInMs(1000);
  std::chrono::milliseconds timePerInstruction(timeOrderInMs.count() /
                                               instructionsPerSecond);

  //--------------//
  mem.setByte("00", "11");
  mem.setByte("1FF", "FF");
  mem.print();
  mem.printInHex();

  std::cout << fetch(mem) << std::endl;

  // while (true) {
  //   auto execStart = std::chrono::steady_clock::now();

  //   std::cout << "FETCH" << std::endl;
  //   std::cout << "DECODE" << std::endl;
  //   std::cout << "EXECUTE" << std::endl;

  //   auto execEnd = std::chrono::steady_clock::now();
  //   auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
  //       execEnd - execStart);

  //   if (elapsed < timePerInstruction) {
  //     auto remainingTime = timePerInstruction - elapsed;
  //     std::this_thread::sleep_for(remainingTime);
  //   }
  //   if (disp.getReprint() == true) {
  //     disp.protoPrint();
  //   }
  // }
  return 0;
}