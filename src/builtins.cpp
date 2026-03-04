#include "shell.hpp"

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <print>

namespace shell::builtins {

	// ReturnCodes exit() {
	//
	// }
	
	ReturnCodes echo(const std::string& text) {
		std::cout << text << std::endl;

		return ReturnCodes::Success;
	}

	ReturnCodes type(const std::string& command) {
		for (auto iter = std::begin(builtins); iter != std::end(builtins); iter++) {
			if (*iter != command)
				continue;

			std::cout << command << " is a shell builtin" << std::endl;
			return ReturnCodes::Success;
		}

		std::cout << command << ": not found" << std::endl;
		return ReturnCodes::Failure;
	}

}
