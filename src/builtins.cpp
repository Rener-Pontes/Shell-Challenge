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

	ReturnCodes type(const std::string& command) {
		for (auto iter = std::begin(builtins); iter != std::end(builtins); iter++) {
			if (*iter != command)
				continue;

			std::cout << command << " is a shell builtin" << std::endl;
			return ReturnCodes::Success;
		}

		std::vector<fs::directory_entry> path = getSystemPath();
		for (const auto& dir : path) {
			using fs::perms;

			fs::path file_path = dir.path() / command;
			if (! fs::exists(file_path)) continue;

			auto file_perms = fs::status(file_path).permissions();
			const fs::perms exec_perms = perms::owner_exec | perms::group_exec | perms::others_exec;
			if (perms::none == (file_perms & exec_perms)) continue;


			std::cout << command << " is " << file_path.string() << std::endl;
			return ReturnCodes::Success;
		}


		std::cout << command << ": not found" << std::endl;
		return ReturnCodes::Failure;
	}

}
