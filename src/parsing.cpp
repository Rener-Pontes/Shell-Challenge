#include "shell.hpp"
#include <fcntl.h>

inline bool isRedirection(const std::string_view& token) {
	return token == ">" || token == ">>"
		|| token == "1>" || token == "1>>"
		|| token == "2>" || token == "2>>";
}

namespace shell {

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
						pos++; // Consumed the extra '>'
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

		return args;
	}

	Command parseArguments(const Args& args) {
		size_t args_size = args.size();
		if (args_size == 0) {
			return {};
		}

		Command cmd{.cmd = args[0]};

		for (size_t i = 1; i < args_size; i++) {
			const std::string& argument = args[i];
			if (::isRedirection(argument)) {
				if (i+1 >= args_size)
					break;

				Redirection::Target target = argument.contains('2') ?
						Redirection::Target::Stderr : Redirection::Target::Stdout;
				Redirection::Mode mode = argument.contains(">>") ?
						Redirection::Mode::Append : Redirection::Mode::Truncate;

				int fd = open(
					args[++i].c_str(),
					O_CREAT | O_WRONLY | (mode == Redirection::Mode::Append ? O_APPEND : O_TRUNC),
					0644
				);

				cmd.redirection = {fd, target, mode};
			} else {
				cmd.args.emplace_back(argument);
			}
		}

		return cmd;
	}

}
