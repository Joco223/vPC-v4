#pragma once

#include <string>
#include <fstream>
#include <iostream>

namespace file_handler {
	std::string load_file(std::string file_path) {
		std::ifstream file(file_path);
		if (!file.is_open()) {
			std::cerr << "Error opening file: " << file_path << ".\n"; return "";
		}
		std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		return str;
	}
}