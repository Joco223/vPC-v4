#pragma once

#include <vector>
#include <string>
#include <regex>
#include <algorithm>

namespace parser {
	std::vector<std::string> split(std::string& input, char delimiter) {
		std::vector<std::string> output;
		
		size_t pos = 0;
		std::string token;
		while ((pos = input.find(delimiter)) != std::string::npos) {
				token = input.substr(0, pos);
				output.push_back(token);
				input.erase(0, pos + 1);
		}

		return output;
	}

	std::vector<std::string> tokenize(std::string& input) {
		std::regex comment_reg(";(.)*");
		input = std::regex_replace(input, comment_reg, " ");
		std::replace(input.begin(), input.end(), '\n', ' ');
		std::vector<std::string> output = split(input, ' ');
		output.erase(std::remove(output.begin(), output.end(), ""));
		return output;
	}

	const std::vector<std::string> valid_registers    = {"reg0" , "reg1"    , "reg2" , "reg3", "reg4", "reg5", "reg6", "reg7"};
	const std::vector<std::string> valid_instructions = {"set"  , "add"     , "sub"  , "mlt" , "div" , "mod" , "inc" , "dec" , "bigg", "bigg_eq",
	                                                     "small", "small_eq", "equal", "diff", "jmp" , "jmpz", "jmpo", "call", "ret" , 
																											 "alloc", "allocm"  , "outu" , "outs", "outc"};
	const std::vector<std::string> valid_keywords     = {"func"};

	bool check_tokens(std::vector<std::string>& tokens) {
		std::regex address_reg("^#(\\d)+-(\\d)+$");

		for (auto& i : tokens) {
			if (std::find(valid_registers.begin()   , valid_registers.end()   , i) != valid_registers.end())    continue;
			if (std::find(valid_instructions.begin(), valid_instructions.end(), i) != valid_instructions.end()) continue;
			if (std::find(valid_keywords.begin()    , valid_keywords.end()    , i) != valid_keywords.end())     continue;
			
			if (std::regex_match(i, address_reg)) continue;
			if (i.find_first_not_of( "0123456789" ) == std::string::npos) continue;

			std::cerr << "Token: \"" << i << "\" doesn't match any valid token. Aborting.\n";
			return false;
		}

		return true;
	}
}