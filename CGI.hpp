#pragma once
#include "main.hpp"
#include <map>

class CGI {
private:
	// static char **_env;
	// static const std::string _path = "/Users/cvenkman/Desktop/webserv/www/cgi-bin/a.out";

	std::vector<std::string> _cgiEnvVector;
	char **_env;
public:

	CGI(std::string requestMethod) {
		_cgiEnvVector.push_back("GATEWAY_INTERFACE: CGI/1.1");
		_cgiEnvVector.push_back("SERVER_PROTOCOL: HTTP/1.1");
		_cgiEnvVector.push_back("SERVER_SOFTWARE: WEBSERV");
		_cgiEnvVector.push_back("REQUEST_METHOD: " + requestMethod);
		std::cout << "_----- " << requestMethod << "\n";
	}

	std::string createFileWithScriptOutput() {
		char **argv = (char **)malloc(sizeof(char *) * 4);
		if (!argv)
			return "www/500.html";
		argv[0] = "/Users/cvenkman/Desktop/webserv/www/cgi-bin/a.out";
 		argv[1] = NULL;
 		argv[2] = NULL;

		if (createEnv() == 500)
			return "www/500.html";

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
			// execve(argv[0], argv, env_);
			// exit(1);

			close(pip[0]);
			int fd_in = open("/Users/cvenkman/Desktop/webserv/www/cgi-bin/a.out", O_RDONLY);
			dup2(fd_in, 0);
			int fdd = open("tmp_file", O_RDWR | O_CREAT, 0777);
			dup2(fdd, 1);
			close(fdd);
			execve(argv[0], argv, _env);
			exit(1);
		}
		else if (pid > 0) {
			close(pip[1]);
			close(pip[0]);
			int status;
			waitpid(pid, &status, 0);
		}
		for (int i = 0; _env[i] != NULL; i++)
			free(_env[i]);
		free(_env);
		free(argv);
		return "tmp_file";
	}

	int createEnv() {
		_cgiEnvVector.push_back("PATH_INFO: /Users/cvenkman/Desktop/webserv/www/cgi-bin/a.out");
		_cgiEnvVector.push_back("REMOTE_ADDR: 127.0.0.1");
		_cgiEnvVector.push_back("SCRIPT_NAME: /cgi-bin/a.out");
		_cgiEnvVector.push_back("SERVER_NAME: 127.0.0.1");
		_cgiEnvVector.push_back("SERVER_PORT: 2001");

		_env = (char **)malloc(sizeof(char *) * _cgiEnvVector.size() + 1);
		if (!_env)
			return 500;

		std::vector<std::string>::iterator it = _cgiEnvVector.begin();
		for (int i = 0; it != _cgiEnvVector.end(); it++, i++) {
			_env[i] = strdup((*it).c_str());
		}
		_env[_cgiEnvVector.size()] = NULL;
		return 0;
	}
};









		//cgiEnvMap.push_back("AUTH_TYPE: NULL");
		// cgiEnvMap.push_back("CONTENT_TYPE: NULL");
		// cgiEnvMap.push_back("CONTENT_LENGTH: NULL");
		/*
		cgiEnvMap.push_back("GATEWAY_INTERFACE: CGI/1.1");
		cgiEnvMap.push_back("PATH_INFO: /Users/cvenkman/Desktop/webserv/www/cgi-bin/a.out");
		cgiEnvMap.push_back("PATH_TRANSLATED: /Users/cvenkman/Desktop/webserv/www/cgi-bin/a.out");
		//cgiEnvMap.push_back("QUERY_STRING:");
		cgiEnvMap.push_back("REMOTE_ADDR: 127.0.0.1");

		cgiEnvMap.push_back("REQUEST_METHOD: GET");
		cgiEnvMap.push_back("SCRIPT_NAME: /cgi-bin/a.out");
		cgiEnvMap.push_back("SERVER_NAME: 127.0.0.1");

		cgiEnvMap.push_back("SERVER_PROTOCOL: HTTP/1.1");
		cgiEnvMap.push_back("SERVER_PORT: 2001");
		cgiEnvMap.push_back("SERVER_SOFTWARE: WEBSERV"); */

		//char **_env = (char **)malloc(sizeof(char *) * envVector.size() + 1);
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



				//std::string res;
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