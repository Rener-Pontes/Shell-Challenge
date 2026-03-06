#pragma once

#include <filesystem>
#include <functional>
#include <map>
#include <string>
#include <vector>


namespace fs = std::filesystem;

namespace shell {

	using Args = std::vector<std::string>;

	struct Command {
		std::string cmd;
		Args args;
	};

	enum class ReturnCodes {
		Success,
		Failure,

		Exit,
	};

	enum class InputState {
		None,
		SimpleQuotes
	};

	std::vector<fs::directory_entry> getSystemPath();
	Command parseInput(const std::string_view& input);
	ReturnCodes run(Command& command);

	fs::path returnExecutablePath(const std::string& command);

	namespace builtins {

		inline const std::string builtins[] = {
			"exit",
			"echo",
			"type",
		};

		ReturnCodes echo(const Command& command);
		ReturnCodes type(const Command& command);
		ReturnCodes pwd(const Command& command);
		ReturnCodes cd(const Command& command);

		inline const std::map<std::string, std::function<ReturnCodes(const Command& command)>> builtins_table = {
			{"exit", [](const Command& command)->ReturnCodes { return ReturnCodes::Exit; }},
			{"echo", echo},
			{"type", type},
			{"pwd",  pwd},
			{"cd",   cd},
		};
	}

} // namespace shell
