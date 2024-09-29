#include "../instructions/instructions.hpp"
#include "../components/components.hpp"

void clearScreen(Display &display) { display.setAllPixels(false); }

void jumpTo(uint16_t instruction, Memory &mem) {
  uint16_t newPC = instruction & 0x0FFF;
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
  uint8_t reg = (instruction & 0x0F00) >> 8;
  uint8_t val = (instruction & 0x00FF);
  variableRegs.setReg(reg, val);
}

void addInRegister(uint16_t instruction, components::Registers &variableRegs) {
  uint16_t oneByteMaxVal = 255;

  uint8_t reg = (instruction & 0x0F00) >> 8;
  uint8_t val = (instruction & 0x00FF);
  uint8_t insideRegVal = variableRegs.getReg(reg);
  uint16_t sum =
      static_cast<uint16_t>(val) + static_cast<uint16_t>(insideRegVal);
  if (sum > oneByteMaxVal) {
    variableRegs.setReg(reg, oneByteMaxVal);
  } else {
    variableRegs.setReg(reg, val + insideRegVal);
  }
}

void setIndexRegister(uint16_t instruction, uint16_t &indexReg) {
  indexReg = instruction & 0x0FFF;
}

void displaySprite(uint16_t instruction, components::Registers &variableRegs,
                   components::Memory &mem, components::Display &display,
                   uint16_t &indexReg) {
  constexpr uint8_t SCREEN_WIDTH = 64;
  constexpr uint8_t SCREEN_HEIGHT = 32;

  uint8_t X = (instruction & 0x0F00) >> 8;
  uint8_t Y = (instruction & 0x00F0) >> 4;
  uint8_t N = instruction & 0x000F;

  uint8_t coordX = variableRegs.getReg(X) % SCREEN_WIDTH;
  uint8_t coordY = variableRegs.getReg(Y) % SCREEN_HEIGHT;

  variableRegs.setReg(0xF, 0);
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
    variableRegs.setReg(0xF, 1);
  }
}

void conditional(uint16_t instruction, components::Registers &variableRegs,
                 components::Memory &mem) {
  uint8_t instCode = (instruction & 0xF000) >> 12;
  uint8_t x = (instruction & 0x0F00) >> 8;
  uint8_t nn = instruction & 0x00FF;

  if (instCode == 3 || instCode == 4) {
    bool conditionMet = (instCode == 3) ? (variableRegs.getReg(x) == nn)
                                        : (variableRegs.getReg(x) != nn);
    if (conditionMet) {
      mem.setPC(mem.getPC() + 2);
    }
  }
  if (instCode == 5 || instCode == 9) {
    bool conditionMet = (instCode == 5) ? (variableRegs.getReg(x) == nn)
                                        : (variableRegs.getReg(x) != nn);
    if (conditionMet) {
      mem.setPC(mem.getPC() + 2);
    }
  }
}
