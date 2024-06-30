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

void setIndexRegister(uint16_t instruction, uint16_t indexReg) {
  indexReg = instruction & 0x0FFF;
}

void displaySprite(uint16_t instruction, components::Registers &variableRegs,
                   components::Memory &mem, components::Display &display) {
  // DXYN
  uint8_t X = (instruction & 0x0F00) >> 8;
  uint8_t Y = (instruction & 0x00F0) >> 4;
  uint8_t N = instruction & 0x000F;

  uint16_t coordX = variableRegs.getReg(static_cast<size_t>(X)) & 63;
  uint16_t coordY = variableRegs.getReg(static_cast<size_t>(Y)) & 31;

  // Setting reg VF to 0
  variableRegs.setReg(15, 0);

  for (uint8_t i = 0; i < N; ++i) {
    uint8_t spriteRow = mem.getByte(variableRegs.getReg(0xF) + i);

    for (uint8_t j = 0; j < 8; ++j) {
      if ((spriteRow & (0x80 >> j)) !=
          0) { // Check if the j-th bit of the sprite row is set
        uint16_t x = (coordX + j) % 64;
        uint16_t y = (coordY + i) % 32;

        if (display.getPixel(y, x)) {
          // If the pixel is already on, turn it off and set VF to 1
          variableRegs.setReg(15, 1);
          display.setPixel(y, x, false);
        } else {
          // If the pixel is off, turn it on
          display.setPixel(y, x, true);
        }
      }
    }
  }
}
