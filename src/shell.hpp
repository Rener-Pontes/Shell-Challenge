#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace shell {

	enum class ReturnCodes {
		Success,
		Failure,
	};

	std::vector<fs::directory_entry> getSystemPath();

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
