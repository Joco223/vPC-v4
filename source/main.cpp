#include <iostream>

#include "CPU.h"

int main(int argc, char** argv) {

  CPU cpu;

  function main_fn;
  main_fn.instructions.push_back({0x1D, {2}});
  main_fn.instructions.push_back({0x1E, {0, 1}});
  main_fn.instructions.push_back({0x1E, {1, 1}});
  main_fn.instructions.push_back({0x01, {0, 5}});
  main_fn.instructions.push_back({0x07, {0, 0, 0}});
  main_fn.instructions.push_back({0x01, {0, 10}});
  main_fn.instructions.push_back({0x07, {1, 0, 0}});
  main_fn.instructions.push_back({0x1B, {1, 0, 1}});
  main_fn.instructions.push_back({0x03, {0, 2, 0}});
  main_fn.instructions.push_back({0x1F, {0}});
  main_fn.instructions.push_back({0x01, {0, 0x20}});
  main_fn.instructions.push_back({0x21, {0}});
  main_fn.instructions.push_back({0x03, {0, 3, 0}});
  main_fn.instructions.push_back({0x1F, {0}});

  function two_returns;
  two_returns.instructions.push_back({0x03, {0, 0, 0}});
  two_returns.instructions.push_back({0x03, {1, 1, 0}});
  two_returns.instructions.push_back({0x10, {0}});
  two_returns.instructions.push_back({0x11, {1}});
  two_returns.instructions.push_back({0x07, {0, 0, 0}});
  two_returns.instructions.push_back({0x07, {1, 0, 1}});
  two_returns.instructions.push_back({0x1C, {0, 1}});

  std::vector<function> functions;
  functions.push_back(main_fn);
  functions.push_back(two_returns);

  cpu.load_functions(functions, 0);

  while (!cpu.is_halted()) {
    cpu.tick();
  }

  return 0;
}