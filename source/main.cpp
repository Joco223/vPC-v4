#include <iostream>

#include "CPU.h"

int main(int argc, char** argv) {

  if (argc < 2) {
    std::cerr << "No input program provided\n";
    return -1;
  }

  CPU cpu;

  cpu.load_functions(argv[1]);

  while (!cpu.is_halted()) {
    cpu.tick();
  }

  return 0;
}