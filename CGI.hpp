#pragma once
#include "main.hpp"

#define ERROR_500 "www/500.html"
#define TMP_CGI "tmp_cgi_file.$"

class CGI {
private:
	std::string path;

	std::vector<std::string> cgiEnvVector;
	char **env;
	char **argv;
	std::fstream cgiTmpFile;

	int createEnv() {
		env = (char **)malloc(sizeof(char *) * cgiEnvVector.size() + 1);
		if (!env)
			return 500;

		std::vector<std::string>::iterator it = cgiEnvVector.begin();
		for (int i = 0; it != cgiEnvVector.end(); it++, i++)
			env[i] = strdup((*it).c_str());
		env[cgiEnvVector.size()] = NULL;
		return 0;
	}

	int createArgv() {
		argv = (char **)malloc(sizeof(char *) * 2);
		if (!argv)
			return 500;
		argv[0] = (char *)malloc(sizeof(char) * path.size() + 1);
		if (!argv[0])
			return 500;
		strcpy(argv[0], path.c_str());
 		argv[1] = NULL;
		return 0;
	}
	
	std::string createBodyFromPage(std::string page) {
		if (page == ERROR_500) {
			perror("ERROR CGI: ");
		}
		cgiTmpFile.open(page);
		std::stringstream bufferCgi;
		bufferCgi << cgiTmpFile.rdbuf();
		std::string body = bufferCgi.str();
		cgiTmpFile.close();
		if (page == TMP_CGI)
			remove(TMP_CGI);
		return body;
	}

public:

	CGI(std::string _requestMethod, std::string _path) : env(NULL), argv(NULL) {
		this->path = const_cast<std::string &>(_path);
		/*----обязательно----*/
		cgiEnvVector.push_back("GATEWAY_INTERFACE: CGI/1.1");
		cgiEnvVector.push_back("SERVER_PROTOCOL: HTTP/1.1");
		cgiEnvVector.push_back("SERVER_SOFTWARE: WEBSERV");
		cgiEnvVector.push_back("SERVER_NAME: 127.0.0.1");
		cgiEnvVector.push_back("SERVER_PORT: 2001");
		/*--конец обязательного--*/
		cgiEnvVector.push_back("REQUEST_METHOD: " + _requestMethod);
		cgiEnvVector.push_back("SCRIPT_NAME: a.out");
		cgiEnvVector.push_back("CONTENT_LENGTH: 48");
		cgiEnvVector.push_back("CONTENT_TYPE: text/html");
		/*
		если кидать PATH_INFO то нужно и PATH_TRANSLATED
		cgiEnvVector.push_back("PATH_INFO: /Users/cvenkman/Desktop/webserv/www/cgi-bin/a.out");
		*/
		/*
		cgiEnvVector.push_back("REQUEST_LINE");
		REQUEST_LINE Содержит строку из запроса протокола HTTP. Например:
		GET /cgi-bin/1.cgi HTTP/1.0
		*/
	}

	~CGI() {
		if (env) {
			for (int i = 0; env[i] != NULL; i++)
				free(env[i]);
			free(env);
		}
		if (argv)
			free(argv);
	}

	std::string executeCgiScript() {
		if (createEnv() != 0 || createArgv() != 0)
			return createBodyFromPage(ERROR_500);

		pid_t pid = fork();
		if (pid < 0)
			return createBodyFromPage(ERROR_500);
		else if (pid == 0) {
			// int fd_in = open("www/cgi-bin/a.out", O_RDONLY);
			int fd_out = open(TMP_CGI, O_RDWR | O_CREAT, 0777);

			if (fd_out == -1)
				return createBodyFromPage(ERROR_500);
			if (dup2(fd_out, 1) == -1)
				return createBodyFromPage(ERROR_500);
			if (close(fd_out) == -1)
				return createBodyFromPage(ERROR_500);
			execve(argv[0], argv, env);
			return createBodyFromPage(ERROR_500);
		}
		else if (pid > 0) {
			int status;
			waitpid(pid, &status, 0);
			if (WIFEXITED(status) == 0)
				return createBodyFromPage(ERROR_500);
		}
		return createBodyFromPage(TMP_CGI);
	}
};
