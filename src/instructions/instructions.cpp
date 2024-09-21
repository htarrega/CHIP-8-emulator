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

// void displaySprite(uint16_t instruction, components::Registers &variableRegs,
//                    components::Memory &mem, components::Display &display,
//                    uint16_t &indexReg) {
//   constexpr uint8_t SCREEN_WIDTH = 64;
//   constexpr uint8_t SCREEN_HEIGHT = 32;

//   // Extract X and Y coordinates from the instruction
//   uint8_t X = (instruction & 0x0F00) >> 8;
//   uint8_t Y = (instruction & 0x00F0) >> 4;
//   uint8_t N = instruction & 0x000F;

//   // Calculate the starting coordinates, wrapping if necessary
//   uint8_t coordX = variableRegs.getReg(X) % SCREEN_WIDTH;
//   uint8_t coordY = variableRegs.getReg(Y) % SCREEN_HEIGHT;

//   // Set VF to 0 initially
//   variableRegs.setReg(0xF, 0);

//   bool pixelFlipped = false;

//   for (uint8_t y = 0; y < N; ++y) {
//     uint8_t spriteRow = mem.getByte(indexReg + y);
//     indexReg += 1; // Increment I register after reading a byte

//     for (uint8_t x = coordX; x < SCREEN_WIDTH && x < coordX + 8;
//          ++x) { // Check if X is within screen bounds
//       if ((spriteRow & (0x80 >> (7 - (x - coordX)))) !=
//           0) { // Check if the current bit is set
//         uint8_t yIndex = coordY + y;

//         if (yIndex < SCREEN_HEIGHT) {
//           bool currentPixel = display.getPixel(yIndex, x);
//           display.setPixel(yIndex, x, !currentPixel);

//           if (currentPixel) {
//             pixelFlipped = true;
//           }
//         } else {
//           break; // Stop drawing if we reach the edge of the screen
//         }
//       }
//     }

//     coordX += 8; // Move to the next row's starting X coordinate

//     if (coordY + y >=
//         SCREEN_HEIGHT) { // Check if Y has reached the bottom of the screen
//       break;
//     }
//   }

//   if (pixelFlipped) {
//     variableRegs.setReg(0xF, 1); // Set VF to 1 if any pixels were flipped
//   }
// }

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
}
