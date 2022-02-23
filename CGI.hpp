#pragma once
#include "main.hpp"

class CGI {
private:
	// static char **_env;
	// static const std::string _path = "/Users/cvenkman/Desktop/webserv/www/cgi-bin/hello.c";

public:

	static std::string createEnv() {
		std::vector<std::string> envVector;

		envVector.push_back("AUTH_TYPE: NULL");
		// envVector.push_back("CONTENT_TYPE: NULL");
		// envVector.push_back("CONTENT_LENGTH: NULL");
		envVector.push_back("GATEWAY_INTERFACE: CGI/1.1");
		envVector.push_back("PATH_INFO: /Users/cvenkman/Desktop/webserv/www/cgi-bin/hello.c");
		envVector.push_back("PATH_TRANSLATED: /Users/cvenkman/Desktop/webserv/www/cgi-bin/hello.c");
		envVector.push_back("QUERY_STRING:");
		envVector.push_back("REMOTE_ADDR: 127.0.0.1");

		envVector.push_back("REQUEST_METHOD: GET");
		envVector.push_back("SCRIPT_NAME: /cgi-bin/hello.c");
		envVector.push_back("SERVER_NAME: 127.0.0.1");

		envVector.push_back("SERVER_PROTOCOL: HTTP/1.1");
		envVector.push_back("SERVER_PORT: 2001");
		envVector.push_back("SERVER_SOFTWARE: WEBSERV");

		char **_env = (char **)malloc(sizeof(char *) * envVector.size() + 1);
		// _env[0] = "AUTH_TYPE: NULL";
		// // _env[] = "CONTENT_TYPE: NULL";
		// // _env[] = "CONTENT_LENGTH: NULL";
		// _env[1] = "GATEWAY_INTERFACE: CGI/1.1";
		// _env[2] = "PATH_INFO: " + _path.c_str();
		// _env[3] = "PATH_TRANSLATED: " + _path.c_str();
		// _env[4] = "QUERY_STRING:";
		// _env[5] = "REMOTE_ADDR: 127.0.0.1";

		// _env[6] = "REQUEST_METHOD: GET";
		// _env[7] = "SCRIPT_NAME: /cgi-bin/hello.c";
		// _env[8] = "SERVER_NAME: 127.0.0.1";

		// _env[9] = "SERVER_PROTOCOL: HTTP/1.1";
		// _env[10] = "SERVER_PORT: 2001";
		// _env[11] = "SERVER_SOFTWARE: WEBSERV";
		// _env[12] = NULL;

		for (int i = 0; i < envVector.size(); i++) {
			_env[i] = strdup(envVector[i].c_str());
		}
		_env[envVector.size()] = NULL;

		std::string res;
		res = "<!DOCTYPE html>\n\
			<html>\n\
				<head>\n\
					<title>CGI</title>\n\
				</head>\n\
				<body>\n\
					<h1>";
		res += "ENV:";
		res += "</h1>\n";

		for (int i = 0; i < envVector.size(); i++) {
			res += _env[i];
			res += "<br>\n";
		}

		res += "</body>\n\
		</html>\n";

		int pip[2];
		pipe(pip);
		pid_t pid = fork();

		if (pid == 0) {
			// chdir(file_path_.substr(0, file_path_.find_last_of('/')).c_str());
			// chdir()
			
			// close(pip[1]);
			// dup2(pip[0], 0);
			// dup2(tmp_file_.getFd(), 1);
			// close(pip[0]);
			// execve(argv_[0], argv_, env_);
			// exit(1);
		}
		else if (pid > 0) {
			close(pip[0]);
			close(pip[0]);
			int status;
			waitpid(pid, &status, 0);
		}
		return res;
	}
};
