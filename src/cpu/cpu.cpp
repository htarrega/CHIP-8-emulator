#include "cpu.hpp"

#include <cstdlib>
#include <iostream>

uint16_t fetch(components::Memory &mem) {
  uint16_t pc = mem.getPC();
  uint8_t highByte = mem.getByte(pc);
  uint8_t lowByte = mem.getByte(pc + 1);
  mem.setPC(pc + 2);
  return (static_cast<uint16_t>(highByte) << 8) | lowByte;
}

void decodeAndExecute(uint16_t instruction, components::Display &disp,
                      components::Memory &mem, std::stack<uint16_t> &stack,
                      components::Registers &variableRegs, uint16_t &indexReg,
                      Timer &timerDelay, Timer &timerSound) {
  uint8_t instCode = (instruction >> 12) & 0x0F;
  switch (instCode) {
    case 0x0:
      if ((instruction & 0x00FF) == 0xE0) {
        clearScreen(disp);
      } else {
        retFromSubroutine(mem, stack);
      }
      break;
    case 0x1:
      jumpTo(instruction, mem);
      break;
    case 0x2:
      callSubroutine(instruction, mem, stack);
      break;
    case 0x3:
    case 0x4:
    case 0x5:
    case 0x9:
      conditional(instruction, variableRegs, mem);
      break;
    case 0x6:
      setRegister(instruction, variableRegs);
      break;
    case 0x7:
      addInRegister(instruction, variableRegs);
      break;
    case 0x8:
      arithmetic(instruction, variableRegs);
      break;
    case 0xA:
      setIndexRegister(instruction, indexReg);
      break;
    case 0xB:
      jumpOffset(instruction, variableRegs, mem);
      break;
    case 0xC:
      random(instruction, variableRegs);
      break;
    case 0xD:
      displaySprite(instruction, variableRegs, mem, disp, indexReg);
      break;
    case 0xE:
      skipInst(instruction, variableRegs, mem);
      break;
    case 0xF:
      chooseFCodeFunc(instruction, variableRegs, mem, indexReg, timerDelay,
                      timerSound);
      break;
    default:
      std::cout << "Opcode does not exist." << std::endl;
      exit(0);
  }
}
