#include "shell.hpp"

#include <filesystem>
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

}
