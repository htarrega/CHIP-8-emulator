#include <SDL.h>

#include <atomic>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stack>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "components/components.hpp"
#include "cpu/cpu.hpp"

const int CHIP8_WIDTH = 64;
const int CHIP8_HEIGHT = 32;
const int WINDOW_SCALE = 10;
const int WINDOW_WIDTH = CHIP8_WIDTH * WINDOW_SCALE;
const int WINDOW_HEIGHT = CHIP8_HEIGHT * WINDOW_SCALE;

int main(int argc, char **argv) {
  std::cout << "I'm EMU!" << std::endl;

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError()
              << std::endl;
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow(
      "CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

  if (!window) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError()
              << std::endl;
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError()
              << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  components::Memory mem;
  components::Display disp;
  components::Registers variableRegs;
  uint16_t indexReg;
  std::stack<uint16_t> stack;
  Timer timerDelay(true), timerSound(false);
  timerDelay.start(1000 / 60, "timerDelay");
  timerSound.start(1000 / 60, "timerSound");

  int instructionsPerSecond = 700;
  std::chrono::milliseconds timeOrderInMs(1000);
  std::chrono::milliseconds timePerInstruction(timeOrderInMs.count() /
                                               instructionsPerSecond);

  mem.loadBinary("../binaries/");

  bool running = true;
  SDL_Event event;

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
    }

    auto execStart = std::chrono::steady_clock::now();

    auto instruction = fetch(mem);
    decodeAndExecute(instruction, disp, mem, stack, variableRegs, indexReg,
                     timerDelay, timerSound);

    auto execEnd = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        execEnd - execStart);

    if (elapsed < timePerInstruction) {
      auto remainingTime = timePerInstruction - elapsed;
      std::this_thread::sleep_for(remainingTime);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Black background
    SDL_RenderClear(renderer);

    for (int x = 0; x < 2 * CHIP8_HEIGHT; ++x) {
      for (int y = 0; y < CHIP8_WIDTH; ++y) {
        if (disp.getPixel(y, x)) {
          SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        } else {
          SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        }
        SDL_Rect rect = {x * WINDOW_SCALE, y * WINDOW_SCALE, WINDOW_SCALE,
                         WINDOW_SCALE};
        SDL_RenderFillRect(renderer, &rect);
      }
    }

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
