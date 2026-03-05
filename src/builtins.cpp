#include "shell.hpp"

#include <filesystem>
#include <iostream>
#include <iterator>
#include <vector>

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
