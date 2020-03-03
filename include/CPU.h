#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;

struct instruction {
	byte op_code;
	std::vector<dword> arguments;
};

struct function {
	dword instruction_counter = 0;
  std::vector<instruction> instructions;
	std::vector<std::vector<dword>> memory;
	std::vector<dword> old_registers;
};

class CPU {
private:
	std::vector<dword> registers;
	std::vector<std::vector<dword>> global_memory;
	bool halt;

	std::vector<function> function_templates;
	std::vector<function> functions;

  void process();

public:
	CPU();

	void load_functions(const std::string input_file_path);
	void load_functions(std::vector<function> functions, int main_function);
	void tick();

	bool is_halted() const;
};