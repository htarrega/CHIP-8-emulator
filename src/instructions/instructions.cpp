#include <cstdint>
#include <fcntl.h>
#include <iostream>
#include <linux/input.h>
#include <random>
#include <stack>
#include <stdio.h>
#include <unistd.h>

#include "../components/components.hpp"
#include "../instructions/instructions.hpp"

constexpr uint8_t FLAG = 15;

void clearScreen(Display &display) { display.setAllPixels(false); }

void jumpTo(uint16_t instruction, Memory &mem) {
  const uint16_t newPC = instruction & 0x0FFF;
  mem.setPC(newPC);
}

void callSubroutine(uint16_t instruction, Memory &mem,
                    std::stack<uint16_t> &stack) {
  stack.push(mem.getPC());
  jumpTo(instruction, mem);
}

void retFromSubroutine(Memory &mem, std::stack<uint16_t> &stack) {
  if (stack.empty()) {
    std::cerr << "Error: Stack is empty, cannot return from subroutine"
              << std::endl;
    exit(1);
  }

  uint16_t ret = stack.top();
  stack.pop();
  mem.setPC(ret);
}

void setRegister(uint16_t instruction, components::Registers &variableRegs) {
  const uint8_t reg = (instruction & 0x0F00) >> 8;
  const uint8_t val = (instruction & 0x00FF);
  variableRegs.setReg(reg, val);
}

void addInRegister(uint16_t instruction, components::Registers &variableRegs) {
  const uint8_t reg = (instruction & 0x0F00) >> 8;
  const uint8_t val = (instruction & 0x00FF);

  const uint8_t insideRegVal = variableRegs.getReg(reg);

  variableRegs.setReg(reg, insideRegVal + val);
}

void setIndexRegister(uint16_t instruction, uint16_t &indexReg) {
  indexReg = instruction & 0x0FFF;
}

void displaySprite(uint16_t instruction, components::Registers &variableRegs,
                   components::Memory &mem, components::Display &display,
                   uint16_t &indexReg) {
  constexpr uint8_t SCREEN_WIDTH = 64;
  constexpr uint8_t SCREEN_HEIGHT = 32;

  const uint8_t X = (instruction & 0x0F00) >> 8;
  const uint8_t Y = (instruction & 0x00F0) >> 4;
  const uint8_t N = instruction & 0x000F;

  const uint8_t coordX = variableRegs.getReg(X) % SCREEN_WIDTH;
  const uint8_t coordY = variableRegs.getReg(Y) % SCREEN_HEIGHT;

  variableRegs.setReg(FLAG, 0);
  bool pixelFlipped = false;

  for (uint8_t y = 0; y < N; ++y) {
    uint8_t spriteRow = mem.getByte(indexReg + y);

    for (uint8_t x = 0; x < 8; ++x) {
      uint8_t xIndex = (coordX + x) % SCREEN_WIDTH;
      uint8_t yIndex = (coordY + y) % SCREEN_HEIGHT;

      if ((spriteRow & (0x80 >> x)) != 0) {
        bool currentPixel = display.getPixel(yIndex, xIndex);
        display.setPixel(yIndex, xIndex, !currentPixel);

        if (currentPixel) {
          pixelFlipped = true;
        }
      }
    }
  }

  if (pixelFlipped) {
    variableRegs.setReg(FLAG, 1);
  }
}

void conditional(uint16_t instruction, components::Registers &variableRegs,
                 components::Memory &mem) {
  const uint8_t instCode = (instruction & 0xF000) >> 12;
  const uint8_t x = (instruction & 0x0F00) >> 8;
  const uint8_t nn = instruction & 0x00FF;
  const uint8_t y = (instruction & 0x00F0) >> 4;

  const uint8_t regXVal = variableRegs.getReg(x);

  bool conditionMet = false;

  switch (instCode) {
  case 3:
    conditionMet = (regXVal == nn);
    break;
  case 4:
    conditionMet = (regXVal != nn);
    break;
  case 5: {
    const uint8_t regYVal = variableRegs.getReg(y);
    conditionMet = (regXVal == regYVal);
    break;
  }
  case 9: {
    const uint8_t regYVal = variableRegs.getReg(y);
    conditionMet = (regXVal != regYVal);
    break;
  }
  default:
    return;
  }

  if (conditionMet) {
    mem.setPC(mem.getPC() + 2);
  }
}

void arithmetic(uint16_t instruction, components::Registers &variableRegs) {
  const uint8_t x = (instruction & 0x0F00) >> 8;
  const uint8_t y = (instruction & 0x00F0) >> 4;
  const uint8_t subinst = (instruction & 0x000F);

  switch (subinst) {
  case 0:
    variableRegs.setReg(x, variableRegs.getReg(y));
    break;
  case 1:
    variableRegs.setReg(x, variableRegs.getReg(x) | variableRegs.getReg(y));
    break;
  case 2:
    variableRegs.setReg(x, variableRegs.getReg(x) & variableRegs.getReg(y));
    break;
  case 3:
    variableRegs.setReg(x, variableRegs.getReg(x) ^ variableRegs.getReg(y));
    break;
  case 4: {
    const uint16_t sum = variableRegs.getReg(x) + variableRegs.getReg(y);
    variableRegs.setReg(x, sum & 0xFF);
    variableRegs.setReg(FLAG, sum > 255 ? 1 : 0);
    break;
  }
  case 5: {
    const uint8_t Vx = variableRegs.getReg(x);
    const uint8_t Vy = variableRegs.getReg(y);
    uint8_t result = Vx - Vy;
    variableRegs.setReg(x, result);
    variableRegs.setReg(FLAG, (Vx >= Vy) ? 1 : 0);
    break;
  }

  case 6: {
    const uint8_t Vx = variableRegs.getReg(x);
    variableRegs.setReg(x, Vx >> 1);
    variableRegs.setReg(FLAG, Vx & 0x1);
    break;
  }

  case 7: {
    const uint8_t Vx = variableRegs.getReg(x);
    const uint8_t Vy = variableRegs.getReg(y);
    uint8_t result = Vy - Vx;
    variableRegs.setReg(x, result);
    variableRegs.setReg(FLAG, (Vy >= Vx) ? 1 : 0);
    break;
  }
  case 0xE: {
    const uint8_t Vx = variableRegs.getReg(x);
    variableRegs.setReg(x, (Vx << 1) & 0xFF);
    variableRegs.setReg(FLAG, (Vx & 0x80) >> 7);
    break;
  }
  default:
    std::cerr << "Unknown arithmetic operation: 0x" << std::hex << (int)subinst
              << std::endl;
    return;
  }
}

void jumpOffset(uint16_t instruction, components::Registers &variableRegs,
                components::Memory &mem) {

  const uint16_t newPC = instruction & 0x0FFF + variableRegs.getReg(0);
  mem.setPC(newPC);
}

void random(uint16_t instruction, components::Registers &variableRegs) {
  const uint8_t x = (instruction & 0x0F00) >> 8;
  const uint8_t nn = instruction & 0x00FF;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint8_t> distrib(0, nn);

  uint8_t randomValue = distrib(gen);
  variableRegs.setReg(x, randomValue);
}

void modTimer(uint16_t instruction, components::Registers &variableRegs,
              components::Timer &timer) {
  const uint8_t x = (instruction & 0x0F00) >> 8;
  const uint8_t vv = instruction & 0x00FF;
  if (vv == 7) {
    variableRegs.setReg(x, timer.getValue());
  } else {
    timer.setValue(variableRegs.getReg(x));
  }
}

void addToIndex(uint16_t instruction, components::Registers &variableRegs,
                uint16_t &indexReg) {
  const uint8_t x = (instruction & 0x0F00) >> 8;
  const uint32_t sum = indexReg + variableRegs.getReg(x);
  if (sum > UINT16_MAX) {
    variableRegs.setReg(FLAG, 1);
  }
  indexReg = indexReg + variableRegs.getReg(x);
}

Key getKeyPressed() {
  struct input_event ev;
  ssize_t n;
  int fd = open("/dev/input/eventX", O_RDONLY);

  if (fd == -1) {
    perror("Error opening input device");
    return Key::Invalid;
  }

  n = read(fd, &ev, sizeof(ev));
  close(fd);

  if (n == sizeof(ev) && ev.type == EV_KEY && ev.value == 1) {
    return translateCharToKey(ev.code);
  }

  return Key::Invalid;
}

void setKeyPressed(uint16_t instruction, components::Registers &variableRegs) {
  Key key = Key::Invalid;
  uint8_t X = (instruction & 0x0F00) >> 8;

  while (key == Key::Invalid) {
    key = getKeyPressed();
  }

  variableRegs.setReg(X, translateKeyToChar(key));
}

void skipInst(uint16_t instruction, components::Registers &variableRegs,
              components::Memory &mem) {

  const uint8_t x = (instruction & 0x0F00) >> 8;
  const uint8_t code = (instruction & 0x00F0) >> 4;

  Key pressedK = getKeyPressed();

  if (code == 9) {
    if (variableRegs.getReg(x) == translateKeyToChar(pressedK)) {
      mem.setPC(mem.getPC() + 2);
    }
  } else {
    if (variableRegs.getReg(x) != translateKeyToChar(pressedK)) {
      mem.setPC(mem.getPC() + 2);
    }
  }
}

void fontCharacter(uint16_t instruction, components::Registers &variableRegs,
                   uint16_t &indexReg) {
  const uint8_t x = (instruction & 0x0F00) >> 8;
  // TODO:set into a variable extracted from config file
  const uint16_t memoryPos = 4096 + variableRegs.getReg(x) * 5;
  indexReg = memoryPos;
}

void binaryDecimalConv(uint16_t instruction,
                       components::Registers &variableRegs, uint16_t &indexReg,
                       components::Memory &memory) {
  const uint8_t x = (instruction & 0x0F00) >> 8;
  uint8_t num = variableRegs.getReg(x);
  const uint8_t hundreds = num / 100;
  const uint8_t tens = (num % 100) / 10;
  const uint8_t ones = num % 10;

  memory.setByte(indexReg, hundreds);
  memory.setByte(indexReg + 1, tens);
  memory.setByte(indexReg + 2, ones);
}

void storeToMemory(uint16_t instruction, components::Registers &variableRegs,
                   components::Memory &mem, uint16_t &indexReg) {
  const uint8_t x = (instruction & 0x0F00) >> 8;
  for (uint8_t i = 0; i <= x; i++) {
    mem.setByte(indexReg + i, variableRegs.getReg(i));
  }
}

void loadFromMemory(uint16_t instruction, components::Registers &variableRegs,
                    components::Memory &mem, uint16_t &indexReg) {
  const uint8_t x = (instruction & 0x0F00) >> 8;
  for (uint8_t i = 0; i <= x; i++) {
    variableRegs.setReg(i, mem.getByte(indexReg + i));
  }
}

void chooseFCodeFunc(uint16_t instruction, components::Registers &variableRegs,
                     components::Memory &mem, uint16_t &indexReg,
                     Timer &timerDelay, Timer &timerSound) {
  const uint8_t A = (instruction & 0x00F0) >> 4;
  const uint8_t B = instruction & 0x000F;

  if (A == 0x0) {
    if (B == 0x7) {
      modTimer(instruction, variableRegs, timerDelay);
    } else {
      setKeyPressed(instruction, variableRegs);
    }
    return;
  }
  if (A == 0x1) {
    if (B == 0x5) {
      modTimer(instruction, variableRegs, timerDelay);
    }
    if (B == 0x8) {
      modTimer(instruction, variableRegs, timerSound);
    } else {
      addToIndex(instruction, variableRegs, indexReg);
    }
    return;
  }
  if (A == 0x2) {
    fontCharacter(instruction, variableRegs, indexReg);
    return;
  }
  if (A == 0x3) {
    binaryDecimalConv(instruction, variableRegs, indexReg, mem);
    return;
  }
  if (A == 0x5) {
    storeToMemory(instruction, variableRegs, mem, indexReg);
    return;
  }
  if (A == 0x6) {
    loadFromMemory(instruction, variableRegs, mem, indexReg);
    return;
  }
}