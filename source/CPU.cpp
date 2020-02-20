#include "CPU.h"

CPU::CPU() : halt(false) {
	registers.resize(8); //8 general purpose registers
}

void CPU::load_functions(const std::string input_file_path) {
	std::fstream input_file(input_file_path, std::ios::in);
	std::string line;

	int main_function = -1;
	bool first = true;
	bool on_memory = true;
	function new_function;
	while (std::getline(input_file, line)) {
		if (first) {
			main_function = std::stoi(line);
			first = false;
		}else{
			if (line == "###") {
				function_templates.push_back(new_function);
				new_function.instructions.clear();
			}else if(line == "---") {
				on_memory = false;
			}else{
				if (line != "") {
					if (on_memory) {
						std::istringstream iss(line);
						std::string word;
						std::vector<dword> memory_vector;
						while (std::getline(iss, word, ' ')) {
							if (word != "") {
								memory_vector.push_back(std::stoi(word));
							}
						}
						new_function.memory.push_back(memory_vector);
					}else{
						std::istringstream iss(line);
						std::string word;
						instruction new_instruction;
						bool first_ins = true;
						while (std::getline(iss, word, ' ')) {
							if (word != ""){
								if (first_ins) {
									new_instruction.op_code = std::stoi(word);
									first_ins = false;
								}else{
									new_instruction.arguments.push_back(std::stoi(word));
								}
							}
						}
						new_function.instructions.push_back(new_instruction);
					}
				}
			}	
		}
	}

	functions.push_back(function_templates[main_function]);
}

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

		case 0x01: registers[c_ins.arguments[0]]                                                         = c_ins.arguments[1]; break;
		case 0x02: registers[c_ins.arguments[0]]                                                         = registers[c_ins.arguments[1]]; break;
		case 0x03: registers[c_ins.arguments[0]]                                                         = functions.back().memory[c_ins.arguments[1]][c_ins.arguments[2]]; break;
		case 0x04: registers[c_ins.arguments[0]]                                                         = functions.back().memory[registers[c_ins.arguments[1]]][c_ins.arguments[2]]; break;
		case 0x05: registers[c_ins.arguments[0]]                                                         = functions.back().memory[c_ins.arguments[1]][registers[c_ins.arguments[2]]]; break;
		case 0x06: registers[c_ins.arguments[0]]                                                         = functions.back().memory[registers[c_ins.arguments[1]]][registers[c_ins.arguments[2]]]; break;
		case 0x07: functions.back().memory[c_ins.arguments[0]][c_ins.arguments[1]]                       = registers[c_ins.arguments[2]]; break;
		case 0x08: functions.back().memory[registers[c_ins.arguments[0]]][c_ins.arguments[1]]            = registers[c_ins.arguments[2]]; break;
		case 0x09: functions.back().memory[c_ins.arguments[0]][registers[c_ins.arguments[1]]]            = registers[c_ins.arguments[2]]; break;
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

		case 0x12: registers[c_ins.arguments[2]] = registers[c_ins.arguments[0]]  > registers[c_ins.arguments[1]]; break;
		case 0x13: registers[c_ins.arguments[2]] = registers[c_ins.arguments[0]] >= registers[c_ins.arguments[1]]; break;
		case 0x14: registers[c_ins.arguments[2]] = registers[c_ins.arguments[0]]  < registers[c_ins.arguments[1]]; break;
		case 0x15: registers[c_ins.arguments[2]] = registers[c_ins.arguments[0]] <= registers[c_ins.arguments[1]]; break;
		case 0x16: registers[c_ins.arguments[2]] = registers[c_ins.arguments[0]] == registers[c_ins.arguments[1]]; break;
		case 0x17: registers[c_ins.arguments[2]] = registers[c_ins.arguments[0]] != registers[c_ins.arguments[1]]; break;
		
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
			registers.clear();
			registers.resize(8);
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
		case 0x1D: functions.back().memory.resize(c_ins.arguments[0]); break;
		case 0x1E: functions.back().memory.resize(registers[c_ins.arguments[0]]); break;

		case 0x26: registers[c_ins.arguments[0]] = functions.back().memory[c_ins.arguments[1]].size(); break;
		case 0x29: registers[c_ins.arguments[0]] = functions.back().memory.size(); break;

		case 0x1F: functions.back().memory[c_ins.arguments[0]].resize(c_ins.arguments[1]); break;
		case 0x20: functions.back().memory[registers[c_ins.arguments[0]]].resize(c_ins.arguments[1]); break;
		case 0x21: functions.back().memory[c_ins.arguments[0]].resize(registers[c_ins.arguments[1]]); break;
		case 0x22: functions.back().memory[registers[c_ins.arguments[0]]].resize(registers[c_ins.arguments[1]]); break;

		case 0x23: std::cout << registers[c_ins.arguments[0]]; break;
		case 0x24: std::cout << (int)registers[c_ins.arguments[0]]; break;
		case 0x25: std::cout << (char)registers[c_ins.arguments[0]]; break;
		case 0x27: { 
			std::string input;
			std::cin >> input;
			functions.back().memory[c_ins.arguments[0]].clear();
			for (auto& i : input)
				functions.back().memory[c_ins.arguments[0]].push_back(static_cast<int>(i));
			break; }
		case 0x28: {
			std::string value = std::to_string(registers[c_ins.arguments[0]]);
			functions.back().memory[c_ins.arguments[1]].clear();
			for (auto& i : value)
				functions.back().memory[c_ins.arguments[1]].push_back(static_cast<int>(i));
			break; }

		default:
			halt = true;
			std::cerr << "Unknown instruction: 0x" << std::hex << c_ins.op_code << '\n';
	}

	functions.back().instruction_counter++;
}

void CPU::tick() {
	if (!halt) {
		if (functions.back().instruction_counter >= functions.back().instructions.size()) {
			if (functions.size() == 1) {
				std::cout << "\nFinished executing.\n";
				halt = true;
				return;
			}else{
				functions.pop_back();
			}
		}
		process();
	}
}

bool CPU::is_halted() const {
	return halt;
}