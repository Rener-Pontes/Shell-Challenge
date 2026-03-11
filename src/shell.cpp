#include "shell.hpp"

#include <sys/wait.h>
#include <unistd.h>

#include <iostream>


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

	ReturnCodes run(Command& command) {
		if (builtins::builtins_table.contains(command.cmd)) {
			int target = -1,
				original_target = -1;
			if (command.redirection.fd_output != -1) {
				target = command.redirection.target == Redirection::Target::Stdout ?
					STDOUT_FILENO : STDERR_FILENO;
				original_target = dup(target);
				dup2(command.redirection.fd_output, target);
				close(command.redirection.fd_output);
			}

			ReturnCodes ret_code = builtins::builtins_table.at(command.cmd)(command);

			if (command.redirection.fd_output != -1) {
				dup2(original_target, target);
				close(original_target);
			}

			return ret_code;
		}

		if (fs::path executable = returnExecutablePath(command.cmd); ! executable.empty()) {
			pid_t pid = fork();
			if (pid == 0) {
				if (command.redirection.fd_output != -1) {
					int target = command.redirection.target == Redirection::Target::Stdout ?
							STDOUT_FILENO : STDERR_FILENO;
					dup2(command.redirection.fd_output, target);
					close(command.redirection.fd_output);
				}

				std::vector<char*> argv;
				argv.reserve(command.args.size()+2);
				argv.emplace_back(const_cast<char*>(command.cmd.c_str()));
				for (std::string& arg : command.args) {
					argv.emplace_back(const_cast<char*>(arg.c_str()));
				}
				argv.emplace_back(nullptr);

				execv(executable.c_str(), argv.data());
				_exit(0);
			} else {
				int stats;
				waitpid(pid, &stats, 0);
			}
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
 
