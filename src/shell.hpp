#pragma once

#include <string>

#define SIZE(arr) sizeof(arr) / sizeof(arr[0])

namespace shell {

	enum class ReturnCodes {
		Success,
		Failure,
	};

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
