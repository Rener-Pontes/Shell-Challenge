#include <iostream>
#include <string>

#include "shell.hpp"

int main() {
	// Flush after every std::cout / std:cerr
	std::cout << std::unitbuf;
	std::cerr << std::unitbuf;

	while (true) {
		std::cout << "$ ";

		std::string input;
		std::getline(std::cin, input);
		shell::Command command = shell::parseInput(input);

		shell::ReturnCodes ret = shell::run(command);
		if (ret == shell::ReturnCodes::Exit){
			return 0;
		}
	}
}
