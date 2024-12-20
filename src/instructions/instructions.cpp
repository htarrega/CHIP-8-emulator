#include "../instructions/instructions.hpp"

#include <SDL.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <unistd.h>

#include <cstdint>
#include <iostream>
#include <random>
#include <stack>
#include <unordered_map>

#include "../components/components.hpp"

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
  const size_t SCREEN_HEIGHT = display.getRows();
  const size_t SCREEN_WIDTH = display.getCols();
  const uint8_t SPRITE_WIDTH = 8;
  const uint8_t FLAG_REGISTER = 0xF;

  const uint8_t X = (instruction & 0x0F00) >> 8;
  const uint8_t Y = (instruction & 0x00F0) >> 4;
  const uint8_t N = instruction & 0x000F;

  const uint8_t startRow = variableRegs.getReg(Y) % SCREEN_HEIGHT;
  const uint8_t startCol = variableRegs.getReg(X) % SCREEN_WIDTH;

  variableRegs.setReg(FLAG_REGISTER, 0);
  bool pixelFlipped = false;

  for (uint8_t row = 0; row < N; ++row) {
    uint8_t spriteRow = mem.getByte(indexReg + row);

    for (uint8_t col = 0; col < SPRITE_WIDTH; ++col) {
      uint8_t currentCol = (startCol + col) % SCREEN_WIDTH;
      uint8_t currentRow = (startRow + row) % SCREEN_HEIGHT;

      if ((spriteRow & (0x80 >> col)) != 0) {
        bool currentPixel = display.getPixel(currentRow, currentCol);
        display.setPixel(currentRow, currentCol, !currentPixel);

        if (currentPixel) {
          pixelFlipped = true;
        }
      }
    }
  }

  if (pixelFlipped) {
    variableRegs.setReg(FLAG_REGISTER, 1);
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

static const std::unordered_map<SDL_Keycode, Key> keyMapping = {
    {SDLK_1, Key::One},   {SDLK_2, Key::Two},  {SDLK_3, Key::Three},
    {SDLK_4, Key::C},     {SDLK_q, Key::Four}, {SDLK_w, Key::Five},
    {SDLK_e, Key::Six},   {SDLK_r, Key::D},    {SDLK_a, Key::Seven},
    {SDLK_s, Key::Eight}, {SDLK_d, Key::Nine}, {SDLK_f, Key::E},
    {SDLK_z, Key::A},     {SDLK_x, Key::Zero}, {SDLK_c, Key::B},
    {SDLK_v, Key::F}};

SDL_Keycode translateKeyToSDLKey(Key key) {
  for (const auto &pair : keyMapping) {
    if (pair.second == key) {
      return pair.first;
    }
  }
  return SDLK_UNKNOWN;
}

Key getKeyPressed() {
  SDL_Event event;

  if (SDL_WaitEvent(&event)) {
    if (event.type == SDL_QUIT) {
      exit(0);
    }

    if (event.type == SDL_KEYDOWN) {
      auto it = keyMapping.find(event.key.keysym.sym);
      if (it != keyMapping.end()) {
        return it->second;
      }
    }
  }

  const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
  for (const auto &pair : keyMapping) {
    if (keyboardState[SDL_GetScancodeFromKey(pair.first)]) {
      return pair.second;
    }
  }

  return Key::Invalid;
}

void waitForKeyRelease(Key pressedKey) {
  SDL_Event event;
  const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
  bool keyReleased = false;

  while (!keyReleased) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        exit(0);
      }
    }

    for (const auto &pair : keyMapping) {
      if (pair.second == pressedKey &&
          !keyboardState[SDL_GetScancodeFromKey(pair.first)]) {
        keyReleased = true;
        break;
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void setKeyPressed(uint16_t instruction, components::Registers &variableRegs) {
  uint8_t X = (instruction & 0x0F00) >> 8;

  Key key = Key::Invalid;
  while (key == Key::Invalid) {
    key = getKeyPressed();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  waitForKeyRelease(key);

  uint8_t keyValue = translateKeyToChar(key);
  variableRegs.setReg(X, keyValue);
}

void skipInst(uint16_t instruction, components::Registers &variableRegs,
              components::Memory &mem) {
  uint8_t x = (instruction & 0x0F00) >> 8;
  uint8_t code = (instruction & 0x00FF);

  uint8_t regValue = variableRegs.getReg(x);
  Key expectedKey = static_cast<Key>(regValue);

  SDL_Keycode keycode = translateKeyToSDLKey(expectedKey);
  const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
  bool isPressed = keyboardState[SDL_GetScancodeFromKey(keycode)];

  if (code == 0x9E && isPressed) {
    mem.setPC(mem.getPC() + 2);
  } else if (code == 0xA1 && !isPressed) {
    mem.setPC(mem.getPC() + 2);
  }
}

void fontCharacter(uint16_t instruction, components::Registers &variableRegs,
                   uint16_t &indexReg) {
  const uint8_t x = (instruction & 0x0F00) >> 8;
  // TODO:set into a variable extracted from config file
  const uint16_t memoryPos = variableRegs.getReg(x) * 5;
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