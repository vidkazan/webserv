#pragma once
#include "main.hpp"

class CGI {
private:
	// static char **_env;
	// static const std::string _path = "/Users/cvenkman/Desktop/webserv/www/cgi-bin/a.out";

public:

	static std::string createEnv() {
		std::vector<std::string> envVector;
 
		char **argv_ = (char **)malloc(sizeof(char *) * 4);
		argv_[0] = "/Users/cvenkman/Desktop/webserv/www/cgi-bin/a.out";
 		argv_[1] = NULL;
 		argv_[2] = NULL;

		envVector.push_back("AUTH_TYPE: NULL");
		// envVector.push_back("CONTENT_TYPE: NULL");
		// envVector.push_back("CONTENT_LENGTH: NULL");
		envVector.push_back("GATEWAY_INTERFACE: CGI/1.1");
		envVector.push_back("PATH_INFO: /Users/cvenkman/Desktop/webserv/www/cgi-bin/a.out");
		envVector.push_back("PATH_TRANSLATED: /Users/cvenkman/Desktop/webserv/www/cgi-bin/a.out");
		envVector.push_back("QUERY_STRING:");
		envVector.push_back("REMOTE_ADDR: 127.0.0.1");

		envVector.push_back("REQUEST_METHOD: GET");
		envVector.push_back("SCRIPT_NAME: /cgi-bin/a.out");
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
		// _env[7] = "SCRIPT_NAME: /cgi-bin/a.out";
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
		// res = "<!DOCTYPE html>\n\
		// 	<html>\n\
		// 		<head>\n\
		// 			<title>CGI</title>\n\
		// 		</head>\n\
		// 		<body>\n\
		// 			<h1>";
		// res += "ENV:";
		// res += "</h1>\n";

		for (int i = 0; i < envVector.size(); i++) {
			res += _env[i];
			res += "<br>\n";
		}

		// res += "</body>\n\
		// </html>\n";

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

			close(pip[0]);
			int fd_in = open("/Users/cvenkman/Desktop/webserv/www/cgi-bin/a.out", O_RDONLY);
			dup2(fd_in, 0);
			int fdd = open("tmp_file", O_RDWR | O_CREAT, 0777);
			dup2(fdd, 1);
			close(fdd);
			execve(argv_[0], argv_, _env);
			exit(1);
		}
		else if (pid > 0) {
			close(pip[1]);
			close(pip[0]);
			int status;
			waitpid(pid, &status, 0);
		}
		
		// res = "<!DOCTYPE html>\n\
		// 	<html>\n\
		// 		<head>\n\
		// 			<title>CGI</title>\n\
		// 		</head>\n\
		// 		<body>\n\
		// 			<h1>";
		// res += "ENV:";
		// res += "</h1>\n";

	

		// res += "</body>\n\
		// </html>\n";
		return "tmp_file";
	}
};
