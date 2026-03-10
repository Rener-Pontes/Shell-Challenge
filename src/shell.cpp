#include "shell.hpp"


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

	Args parseInput(const std::string_view& input) {
		size_t input_length = input.length();
		if (input_length == 0) {
			return {};
		}
		size_t pos = input.find_first_not_of(' ');
		if (pos == npos) {
			return {};
		}

		Args args;
		std::string argument;
		InputState state = InputState::None;


		for (; pos < input_length; pos++) {
			char ch = input[pos];
			char explicit_redirection = '\0';

			switch (ch) {
				case ' ':
					if (state != InputState::None)
						break;

					if (! argument.empty()) {
						args.emplace_back(argument);
						argument.clear();
					}

					continue;
				case '\'':
					if (state == InputState::DoubleQuotes)
						break;

					state = state == InputState::SimpleQuotes ?
							InputState::None : InputState::SimpleQuotes;
					continue;
				case '\"':
					if (state == InputState::SimpleQuotes)
						break;

					state = state == InputState::DoubleQuotes ?
							InputState::None : InputState::DoubleQuotes;
					continue;
				case '\\':
					if (state == InputState::SimpleQuotes)
						break;
					
					if (pos+1 < input.length())
						argument.push_back(input[pos+1]);
					pos++; // In theory, one more char was consumed
					continue;
				case '1':
				case '2':
					if (pos+1 >= input_length || input[pos+1] != '>')
						break;

					explicit_redirection = ch;
				case '>':
					if (state != InputState::None)
						break;

					if (! argument.empty()) {
						args.emplace_back(argument);
						argument.clear();
					}
					if (explicit_redirection != '\0') {
						argument.push_back(ch);
						pos++; // Consumed the target fd
					}
					
					argument.push_back('>');
					if (pos + 1 < input_length && input[pos+1] == '>') {
						argument.push_back('>');
					}

					args.emplace_back(argument);
					argument.clear();

					continue;
			}

			argument.push_back(ch);
		}

		if (! argument.empty()) {
			args.emplace_back(argument);
		}

		return {};
	}

	Command parseArguments(const Args& args) {
		size_t args_size = args.size();
		if (args_size == 0) {
			return {};
		}

		Command cmd{.cmd = args[0]};

		for (size_t i = 0; i < args_size; i++) {
			
		}

		return cmd;
	}

	ReturnCodes run(Command& command) {
		return ReturnCodes::Success;
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
 
