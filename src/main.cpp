#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

int main() {
	// Flush after every std::cout / std:cerr
	std::cout << std::unitbuf;
	std::cerr << std::unitbuf;

	while (true) {
		std::cout << "$ ";

		std::string input;
		std::getline(std::cin, input);

		if (auto found = input.find("exit"); found != std::string::npos) {
			return 0;
		} else if (auto found = input.find("echo"); found != std::string::npos) {
			std::string text = input.substr(found + strlen("echo "));
			std::cout << text << std::endl;
		} else {
			std::cout << input << ": command not found" << std::endl;
		}

	}
}
