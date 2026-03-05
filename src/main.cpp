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

		// println("\"{}\"", input.substr(0,4));

		if (input.substr(0, 4) == "exit") {
			return 0;
		} else if (input.substr(0, 4) == "echo") {
			if (input.length() == 4) {
				std::cout << std::endl;
				continue;
			}
			std::string text = input.substr(4+1);
			shell::builtins::echo(text);
		} else if (input.substr(0, 4) == "type") {
			if (input.size() == 4) continue;
			shell::builtins::type(input.substr(4+1));
		} else {
			std::cout << input << ": command not found" << std::endl;
		}
	}
}
