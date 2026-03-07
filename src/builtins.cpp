#include "shell.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>

namespace shell::builtins {

	ReturnCodes echo(const Command& command) {
		std::function ignore = [](std::string text) {
			const std::string_view ignore_list = "\"'\\";
			InputState state = InputState::None;

			for (size_t i = 0; i < text.length(); i++) {
				char ch = text[i];
				if (! ignore_list.contains(ch)) continue;

				switch (ch) {
					case '\'':
						if (state == InputState::DoubleQuotes) {
							break;
						}

						state = state == InputState::None ?
							InputState::SimpleQuotes : InputState::None;

						text.erase(i, 1);
						i--;
						break;

					case '\"': {
						if (state == InputState::SimpleQuotes) {
							break;
						}

						state = state == InputState::None ?
							InputState::DoubleQuotes : InputState::None;

						text.erase(i, 1);
						i--;
						break;
					}
					case '\\': {
						if (state == InputState::SimpleQuotes) break;

						text.erase(i, 1);
						break;
					}
				}
			}

			return text;
		};

		for (const std::string& arg : command.args) {
			std::cout << ignore(arg);
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

		std::cerr << cmd << ": not found" << std::endl;
		return ReturnCodes::Failure;
	}

	ReturnCodes pwd(const Command& command) {
		std::cout << fs::current_path().string() << std::endl;

		return ReturnCodes::Success;
	}

	ReturnCodes cd(const Command &command) {
		fs::path destination{command.args[0]};

		if (std::string dest_str = destination.string(); dest_str.contains('~')) {
			const char* home_var = std::getenv("HOME");
			if (! home_var) {
				return ReturnCodes::Failure;
			}
			
			dest_str.replace(dest_str.find('~'), 1, home_var);
			destination = dest_str;
		}

		if (! fs::exists(destination)) {
			std::cerr << "cd: " << destination.c_str() << ": No such file or directory" << std::endl;
			return ReturnCodes::Failure;
		}
		if (! fs::is_directory(destination)) return ReturnCodes::Failure;

		fs::current_path(destination);

		return ReturnCodes::Success;
	}

}
