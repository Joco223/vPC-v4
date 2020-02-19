#include <iostream>
#include <string>
#include <vector>

#include "file_handler.h"
#include "parser.h"
#include "compiler.h"

int main(int argc, char** argv) {

	if (argc < 3) {
		std::cerr << "Please provide input file and output file names.\n";
		return -1;
	}

	std::string input_program = file_handler::load_file(argv[1]);
	std::vector<parser::token> input_tokens = parser::tokenize(input_program);
	if (!parser::check_tokens(input_tokens))
		return -1;
	compiler::compile(input_tokens, argv[2]);

	return 0;
}