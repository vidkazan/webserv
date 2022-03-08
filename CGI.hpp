#pragma once
#include "main.hpp"
#include <map>
#define CGI_BUFSIZE 65536
class CGI {
private:
	// static char **env;
	// static const std::string _path = "/Users/cvenkman/Desktop/webserv/www/cgi-bin/a.out";

	std::vector<std::string> _cgiEnvVector;
	char **env;
	char **argv;
	std::fstream cgiTmpFile;
public:

	CGI(std::string requestMethod) : env(NULL), argv(NULL) {
		/*----обязательно----*/
		_cgiEnvVector.push_back("GATEWAY_INTERFACE: CGI/1.1");
		_cgiEnvVector.push_back("SERVER_PROTOCOL: HTTP/1.1");
		_cgiEnvVector.push_back("SERVER_SOFTWARE: WEBSERV");
		_cgiEnvVector.push_back("SERVER_NAME: 127.0.0.1");
		_cgiEnvVector.push_back("SERVER_PORT: 2001");
		/*--конец обязательного--*/
		_cgiEnvVector.push_back("REQUEST_METHOD: " + requestMethod);
		std::cout << "_----- " << requestMethod << "\n";

		/*
		если кидать PATH_INFO то нужно и PATH_TRANSLATED
		_cgiEnvVector.push_back("PATH_INFO: /Users/cvenkman/Desktop/webserv/www/cgi-bin/a.out");
		*/
		//_cgiEnvVector.push_back("REMOTE_ADDR: 127.0.0.1"); //IP адрес пользователя
		_cgiEnvVector.push_back("SCRIPT_NAME: a.out");
		_cgiEnvVector.push_back("CONTENT_LENGTH: 48");
		_cgiEnvVector.push_back("CONTENT_TYPE: text/html");
		/*
		_cgiEnvVector.push_back("REQUEST_LINE");
		REQUEST_LINE Содержит строку из запроса протокола HTTP. Например:
		GET /cgi-bin/1.cgi HTTP/1.0
		*/
		
	}

	~CGI() {
		if (env) { // ?? delete env ?
			for (int i = 0; env[i] != NULL; i++)
				free(env[i]);
			free(env);
		}
		if (argv)
			free(argv);
	}

	std::string createBodyFromPage(std::string page) {
		cgiTmpFile.open(page);
		std::stringstream bufferCgi;
		bufferCgi << cgiTmpFile.rdbuf();
		std::string body = bufferCgi.str();
		cgiTmpFile.close();
		// перенести названия файлов в переменную класса static const
		// типа: навзания для файла с ошибками и с tmp
		if (page == "tmp_cgi_file")
			remove("tmp_cgi_file");
		return body;
	}

	std::string executeCgiScript() {
		argv = (char **)malloc(sizeof(char *) * 4);
		if (!argv)
			return createBodyFromPage("www/500.html");
		argv[0] = "/Users/cvenkman/Desktop/webserv/www/cgi-bin/a.out";
 		argv[1] = NULL;
 		argv[2] = NULL;

		if (createEnv() == 500)
			return createBodyFromPage("www/500.html");

		int pip[2];
		pipe(pip);
		pid_t pid = fork();

		if (pid < 0)
			return createBodyFromPage("www/500.html");
		else if (pid == 0) {
			close(pip[0]);
			int fd_in = open("/Users/cvenkman/Desktop/webserv/www/cgi-bin/a.out", O_RDONLY);
			dup2(fd_in, 0);
			int fdd = open("tmp_cgi_file", O_RDWR | O_CREAT, 0777);
			dup2(fdd, 1);
			close(fdd);
			execve(argv[0], argv, env);
			return createBodyFromPage("www/500.html");
		}
		else if (pid > 0) {
			close(pip[1]);
			close(pip[0]);
			int status;
			waitpid(pid, &status, 0);
		}
		return createBodyFromPage("tmp_cgi_file");
	}


	int createEnv() { // to private
		env = (char **)malloc(sizeof(char *) * _cgiEnvVector.size() + 1);
		if (!env)
			return 500;

		std::vector<std::string>::iterator it = _cgiEnvVector.begin();
		for (int i = 0; it != _cgiEnvVector.end(); it++, i++) {
			env[i] = strdup((*it).c_str());
		}
		env[_cgiEnvVector.size()] = NULL;
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

		//char **env = (char **)malloc(sizeof(char *) * envVector.size() + 1);
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