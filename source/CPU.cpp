#include "CPU.h"

CPU::CPU() : halt(false) {
	registers.resize(8); //8 general purpose registers
}

void CPU::load_functions(const std::string input_file) {}

void CPU::load_functions(std::vector<function> function_temp, int main_function) {
	function_templates = function_temp;
	functions.push_back(function_templates[main_function]);
}

void CPU::process() {
	instruction& c_ins = functions.back().instructions[functions.back().instruction_counter];

	switch (c_ins.op_code) {
		case 0x0:
			halt = true;
			break;

		case 0x01: registers[c_ins.arguments[0]] = c_ins.arguments[1]; break;
		case 0x02: registers[c_ins.arguments[0]] = registers[c_ins.arguments[1]]; break;
		case 0x03: registers[c_ins.arguments[0]] = functions.back().memory[c_ins.arguments[1]][c_ins.arguments[2]]; break;
		case 0x04: registers[c_ins.arguments[0]] = functions.back().memory[registers[c_ins.arguments[1]]][c_ins.arguments[2]]; break;
		case 0x05: registers[c_ins.arguments[0]] = functions.back().memory[c_ins.arguments[1]][registers[c_ins.arguments[2]]]; break;
		case 0x06: registers[c_ins.arguments[0]] = functions.back().memory[registers[c_ins.arguments[1]]][registers[c_ins.arguments[2]]]; break;
		case 0x07: functions.back().memory[c_ins.arguments[0]][c_ins.arguments[1]] = registers[c_ins.arguments[2]]; break;
		case 0x08: functions.back().memory[registers[c_ins.arguments[0]]][c_ins.arguments[1]] = registers[c_ins.arguments[2]]; break;
		case 0x09: functions.back().memory[c_ins.arguments[0]][registers[c_ins.arguments[1]]] = registers[c_ins.arguments[2]]; break;
		case 0x0A: functions.back().memory[registers[c_ins.arguments[0]]][registers[c_ins.arguments[1]]] = registers[c_ins.arguments[2]]; break;

		case 0x0B: registers[c_ins.arguments[0]] += registers[c_ins.arguments[1]]; break;
		case 0x0C: registers[c_ins.arguments[0]] -= registers[c_ins.arguments[1]]; break;
		case 0x0D: registers[c_ins.arguments[0]] *= registers[c_ins.arguments[1]]; break;
		case 0x0E:
			if (registers[c_ins.arguments[1]] == 0) {
				std::cerr << "Division with zero error.\n"; halt = true; break;
			}else{
				registers[c_ins.arguments[0]] /= registers[c_ins.arguments[1]];
			}
			break;
		case 0x0F: registers[c_ins.arguments[0]] %= registers[c_ins.arguments[1]]; break;
		case 0x10: registers[c_ins.arguments[0]]++; break;
		case 0x11: registers[c_ins.arguments[0]]--; break;

		case 0x12: registers[c_ins.arguments[0]] = registers[c_ins.arguments[0]] > registers[c_ins.arguments[1]]; break;
		case 0x13: registers[c_ins.arguments[0]] = registers[c_ins.arguments[0]] >= registers[c_ins.arguments[1]]; break;
		case 0x14: registers[c_ins.arguments[0]] = registers[c_ins.arguments[0]] < registers[c_ins.arguments[1]]; break;
		case 0x15: registers[c_ins.arguments[0]] = registers[c_ins.arguments[0]] <= registers[c_ins.arguments[1]]; break;
		case 0x16: registers[c_ins.arguments[0]] = registers[c_ins.arguments[0]] == registers[c_ins.arguments[1]]; break;
		case 0x17: registers[c_ins.arguments[0]] = registers[c_ins.arguments[0]] != registers[c_ins.arguments[1]]; break;
		
		case 0x18: functions.back().instruction_counter = c_ins.arguments[0]; return;
		case 0x19:
			if (registers[c_ins.arguments[1]] == 0) {
				functions.back().instruction_counter = c_ins.arguments[0]; return;
			}
			break;
		case 0x1A:
			if (registers[c_ins.arguments[1]] == 1) {
				functions.back().instruction_counter = c_ins.arguments[0]; return;
			}
			break;

		case 0x1B: 
			functions.back().old_registers = registers;
			functions.push_back(function_templates[c_ins.arguments[0]]); 
			for (int i = 1; i < c_ins.arguments.size(); i++) {
				functions.back().memory.push_back(functions[functions.size()-2].memory[c_ins.arguments[i]]);
			}
			return;
		case 0x1C:
			if (functions.size() == 1 && c_ins.arguments.size() > 0) {
				std::cerr << "Cannot return values from main function.\n";
				return;
			}else{
				for (int i = 0; i < c_ins.arguments.size(); i++) {
					functions[functions.size()-2].memory.push_back(functions.back().memory[c_ins.arguments[i]]);	
				}
				functions.pop_back();
				registers = functions.back().old_registers;
				break;
			}
		case 0x1D: functions.back().memory.resize(functions.back().memory.size() + c_ins.arguments[0]); break;
		case 0x1E: functions.back().memory[c_ins.arguments[0]].resize(functions.back().memory[c_ins.arguments[0]].size() + c_ins.arguments[1]); break;

		case 0x1F: std::cout << registers[c_ins.arguments[0]]; break;
		case 0x20: std::cout << (int)registers[c_ins.arguments[0]]; break;
		case 0x21: std::cout << (char)registers[c_ins.arguments[0]]; break;

		default:
			halt = true;
			std::cerr << "Unknown instruction: 0x" << std::hex << c_ins.op_code << '\n';
	}

	functions.back().instruction_counter++;
}

void CPU::tick() {
	if (!halt) {
		if (functions.back().instruction_counter == functions.back().instructions.size()) {
			if (functions.size() == 1) {
				std::cout << "\nFinished executing.\n";
				halt = true;
				return;
			}else{
				functions.pop_back();
			}
		}
		//std::cout << '#' << functions.back().instruction_counter << '\n';
		process();
	}
}

bool CPU::is_halted() const {
	return halt;
}