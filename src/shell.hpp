#pragma once

#include <filesystem>
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
	};

	std::vector<fs::directory_entry> getSystemPath();

	Command parseInput(const std::string_view& input);

	namespace builtins {

		inline const std::string builtins[] = {
			"exit",
			"echo",
			"type",
		};

		// ReturnCodes exit();
		ReturnCodes echo(const std::string& text);
		ReturnCodes type(const std::string& command);

	}

} // namespace shell
