#include <stack>

#include "../components/components.hpp"

// 00E0
void clearScreen(Display &display);
// 00EE
void retFromSubroutine(Memory &mem, std::stack<uint16_t> &stack);
// 1NNN
void jumpTo(uint16_t instruction, Memory &mem);
// 2NNN
void callSubroutine(uint16_t instruction, Memory &mem,
                    std::stack<uint16_t> &stack);
// 3XNN, 4XNN, 5XY0 & 9XY0
void conditional(uint16_t instruction, components::Registers &variableRegs,
                 components::Memory &mem);
// 6XNN
void setRegister(uint16_t instruction, components::Registers &variableRegs);
// 7XNN
void addInRegister(uint16_t instruction, components::Registers &variableRegs);
// 8XYI
void arithmetic(uint16_t instruction, components::Registers &variableRegs);
// ANNN
void setIndexRegister(uint16_t instruction, uint16_t &indexReg);
// BNNN
void jumpOffset(uint16_t instruction, components::Registers &variableRegs,
                components::Memory &mem);
// CXNN
void random(uint16_t instruction, components::Registers &variableRegs);
// DXYN
void displaySprite(uint16_t instruction, components::Registers &variableRegs,
                   components::Memory &mem, components::Display &display,
                   uint16_t &indexReg);
// EX9E & EXA1
void skipInst(uint16_t instruction, components::Registers &variableRegs,
              components::Memory &mem);
// Function to choose between all code F instructions
void chooseFCodeFunc(uint16_t instruction, components::Registers &variableRegs,
                     components::Memory &mem, uint16_t &indexReg,
                     Timer &timerDelay, Timer &timerSound);
// FX007,FX15 & FX18
void modTimer(uint16_t instruction, components::Registers &variableRegs,
              components::Timer &timer);
// FX1E
void addToIndex(uint16_t instruction, components::Registers &variableRegs,
                uint16_t &indexReg);
// FX0A
void setKeyPressed(uint16_t instruction, components::Registers &variableRegs);
// FX29
void fontCharacter(uint16_t instruction, components::Registers &variableRegs,
                   uint16_t &indexReg);
// FX33
void binaryDecimalConv(uint16_t instruction,
                       components::Registers &variableRegs, uint16_t &indexReg,
                       components::Memory &memory);
//  FX55
void storeToMemory(uint16_t instruction, components::Registers &variableRegs,
                   components::Memory &mem, uint16_t &indexReg);
// FX65
void loadFromMemory(uint16_t instruction, components::Registers &variableRegs,
                    components::Memory &mem, uint16_t &indexReg);
// Util to make the program extract a pressed key
Key getKeyPressed();
