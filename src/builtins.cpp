#include "shell.hpp"

#include <filesystem>
#include <iostream>
#include <vector>

namespace shell::builtins {

	ReturnCodes echo(const Command& command) {
		std::cout << command.args[0];
		for (size_t i = 1; i < command.args.size(); i++) {
			std::cout << ' ' << command.args[i];
		}
		std::cout << std::endl;

		return ReturnCodes::Success;
	}

	ReturnCodes type(const Command& command) {
		const std::string& cmd = command.args[0];
		if (builtins_table.contains(cmd)){
			std::cout << cmd << " is a shell builtin" << std::endl;
			return ReturnCodes::Success;

		}

		if (fs::path file_path = returnExecutablePath(cmd); ! file_path.empty()) {
			std::cout << cmd << " is " << file_path.string() << std::endl;
			return ReturnCodes::Success;
		}

		std::cout << cmd << ": not found" << std::endl;
		return ReturnCodes::Failure;
	}

	ReturnCodes pwd(const Command& command) {
		std::cout << fs::current_path().string() << std::endl;

		return ReturnCodes::Success;
	}

}
