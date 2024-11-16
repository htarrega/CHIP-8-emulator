#ifndef CPU_HPP
#define CPU_HPP

#include "../components/components.hpp"
#include "../instructions/instructions.hpp"

uint16_t fetch(components::Memory &mem);

void decodeAndExecute(uint16_t instruction, components::Display &disp,
                      components::Memory &mem, std::stack<uint16_t> &stack,
                      components::Registers &variableRegs, uint16_t &indexReg,
                      Timer &timerDelay, Timer &timerSound);

#endif  // CPU_HPP