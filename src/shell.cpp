#include "shell.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string_view>
#include <vector>

const size_t npos = std::string::npos;

namespace shell {

	std::vector<fs::directory_entry> getSystemPath() {
		const char* path_var = getenv("PATH");
		if (! path_var)
			return {};
		std::string_view path{path_var};

		std::vector<fs::directory_entry> path_vector = {};
		size_t start_pos, end_pos;

		for (start_pos = 0, end_pos = path.find(':');
			end_pos != npos;
			start_pos = end_pos+1, end_pos = path.find(':', start_pos + 1)
			) {
			path_vector.emplace_back(path.substr(start_pos, end_pos - start_pos));
		}
		path_vector.emplace_back(path.substr(start_pos));

		return path_vector;
	}

	Command parseInput(const std::string_view& input) {
		size_t pos = input.find_first_not_of(' ');
		if (pos == npos) {
			return {};
		}

		size_t end_pos = input.find_first_of(' ', pos);
		std::string cmd{input.substr(pos, end_pos - pos)};

		Args args;
		std::string argument;
		InputState state = InputState::None;
		pos = input.find_first_not_of(' ', end_pos);
		while(pos < input.length()) {
			switch (input[pos]) {
				case ' ': {
					if (state != InputState::None) {
						argument.push_back(' ');
						pos++;
						continue;
					}

					args.emplace_back(argument);
					args.emplace_back(" ");
					argument.clear();

					pos = input.find_first_not_of(' ', pos);
					break;
				};
				case '\'': {
					if (state == InputState::DoubleQuotes){
						argument.push_back('\'');
						pos++;
						break;
					}

					state = state == InputState::None ?
						InputState::SimpleQuotes : InputState::None;

					if (state == InputState::SimpleQuotes && ! argument.empty()) {
						args.push_back(argument);
						argument.clear();
					}

					argument.push_back('\'');
					pos++;

					if (state == InputState::None) {
						args.push_back(argument);
						argument.clear();
					}
					break;
				}
				case '\"': {
					if (state == InputState::SimpleQuotes){
						argument.push_back('\"');
						pos++;
						break;
					}

					state = state == InputState::None ?
						InputState::DoubleQuotes : InputState::None;

					if (state == InputState::SimpleQuotes && ! argument.empty()) {
						args.push_back(argument);
						argument.clear();
					}

					argument.push_back('\"');
					pos++;

					if (state == InputState::None) {
						args.push_back(argument);
						argument.clear();
					}

					break;
				}
				case '\\': {
					if (state != InputState::None) {
						argument.push_back('\\');
						pos++;
						break;
					}

					argument.push_back('\\');
					argument.push_back(input[pos+1]);
					pos += 2;

					break;
				}
				default:
					argument.push_back(input[pos++]);
			}
		}

		if (! argument.empty()) {
			args.emplace_back(argument);
		}

		return {cmd, args};
	}

	ReturnCodes run(Command& command) {
		if (builtins::builtins_table.contains(command.cmd)) {
			return builtins::builtins_table.at(command.cmd)(command);
		}

		if (fs::path executable = returnExecutablePath(command.cmd); ! executable.empty()) {
			std::stringstream cmd{};

			cmd << command.cmd;
			for (const std::string& arg : command.args) {
				cmd << ' ' << arg;
			}

			system(cmd.str().c_str());
			return ReturnCodes::Success;
		}

		std::cerr << command.cmd << ": command not found" << std::endl;
		return ReturnCodes::Failure;
	}


	fs::path returnExecutablePath(const std::string& command) {
		std::vector<fs::directory_entry> path = getSystemPath();
		for (const auto& dir : path) {
			using fs::perms;

			fs::path file_path = dir.path() / command;
			if (! fs::exists(file_path)) continue;

			auto file_perms = fs::status(file_path).permissions();
			const fs::perms exec_perms = perms::owner_exec | perms::group_exec | perms::others_exec;
			if (perms::none == (file_perms & exec_perms)) continue;

			return file_path;
		}

		return "";
	}

}
 
