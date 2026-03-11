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
		shell::Args parsed_args = shell::parseInput(input);
		shell::Command command = shell::parseArguments(parsed_args);

		shell::ReturnCodes ret = shell::run(command);
		if (ret == shell::ReturnCodes::Exit){
			return 0;
		}
	}
}
