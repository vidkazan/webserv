#pragma once
#include "main.hpp"

#define ERROR_500 "www/500.html"
#define TMP_CGI "tmp_cgi_file.$"

class CGI {
private:
	std::string cgiScriptPath;
	std::string cgiScriptFullPath;
	std::vector<std::string> cgiEnvVector;
	char **env;
	char **argv;
	std::fstream cgiTmpFile;

	int initEnv() {
		env = (char **)malloc(sizeof(char *) * cgiEnvVector.size() + 1);
		if (!env)
			return 500;

		std::vector<std::string>::iterator it = cgiEnvVector.begin();
		for (int i = 0; it != cgiEnvVector.end(); it++, i++)
			env[i] = strdup((*it).c_str());
		env[cgiEnvVector.size()] = NULL;
		return 0;
	}

	int initArgv() {
		argv = (char **)malloc(sizeof(char *) * 2);
		if (!argv)
			return 500;
		argv[0] = (char *)malloc(sizeof(char) * cgiScriptPath.size() + 1);
		if (!argv[0])
			return 500;
		strcpy(argv[0], cgiScriptPath.c_str());
 		argv[1] = NULL;
		return 0;
	}
	
	// принимать два параметра - второй с ошибкой
	std::string createBodyFromPage(std::string page) {
		if (page == ERROR_500) {
			perror("ERROR CGI: ");
		}
		cgiTmpFile.open(page);
		std::stringstream bufferCgi;
		if (page == ERROR_500) { // костыль
			bufferCgi << "Content-Type: text/html\n\n";
		}
		bufferCgi << cgiTmpFile.rdbuf();
		std::string body = bufferCgi.str();
		cgiTmpFile.close();
		if (page == TMP_CGI)
			remove(TMP_CGI);
		return body;
	}

	void createFullPathToScript() {
		char cwd[1024];
		getcwd(cwd, sizeof(cwd));
		std::string cwdStr(cwd);
		this->cgiScriptFullPath = cwdStr + "/" + cgiScriptPath;
	}

public:

	CGI(std::string _requestMethod, std::string _cgiScriptPath) : env(NULL), argv(NULL), cgiScriptPath(_cgiScriptPath) {
		//this->path = const_cast<std::string &>(_path);
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
		createFullPathToScript();
		cgiEnvVector.push_back("PATH_INFO: " + cgiScriptFullPath);
		/*
		если кидать PATH_INFO то нужно и PATH_TRANSLATED
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
		//unlink();
	}

	std::string executeCgiScript() {
		if (initEnv() != 0 || initArgv() != 0)
			return createBodyFromPage(ERROR_500);

		int pip[2];
		if (pipe(pip) != 0)
			return createBodyFromPage(ERROR_500);

		pid_t pid = fork();
		if (pid < 0)
			return createBodyFromPage(ERROR_500);

		else if (pid == 0) {
			// chdir

			if (close(pip[1]) == -1)
				return createBodyFromPage(ERROR_500);

			// int fd_in = open(cgiScriptPath.c_str(), O_RDONLY);
			int fd_out = open(TMP_CGI, O_RDWR | O_CREAT, 0777);

			if (fd_out == -1)
				return createBodyFromPage(ERROR_500);
			if (dup2(pip[0], 0) == -1 || dup2(fd_out, 1) == -1)
				return createBodyFromPage(ERROR_500);
			//execve(argv[0], argv, env);
			if (close(fd_out) == -1 || close(pip[0]) == -1 ||
				close(0) == -1)
				return createBodyFromPage(ERROR_500);
			execve(argv[0], argv, env);
			
			exit(EXIT_FAILURE);
		}
		else if (pid > 0) {
			int status;
			close(pip[0]);
			close(pip[1]);
			waitpid(pid, &status, 0);
			if (WIFEXITED(status) == 0)
				return createBodyFromPage(ERROR_500);
		}
		std::string body = createBodyFromPage(TMP_CGI);
		if (strncmp(body.c_str(), "Content-type:", 13) != 0) {
			// из-за этого выходит две ошибки ERROR CGI:
			std::cout << "ERROR CGI: no Content-type: in cgi script\n";
			return createBodyFromPage(ERROR_500);
		}
		return body;
	}
};


// #pragma once
// #include "main.hpp"

// #define ERROR_500 "www/500.html"
// #define TMP_CGI "tmp_cgi_file.$"

// class CGI {
// private:
// 	std::string cgiScriptPath;
// 	std::string cgiScriptFullPath;
// 	std::vector<std::string> cgiEnvVector;
// 	char **env;
// 	char **argv;
// 	std::fstream cgiTmpFile;

// 	int initEnv() {
// 		env = (char **)malloc(sizeof(char *) * cgiEnvVector.size() + 1);
// 		if (!env)
// 			return 500;

// 		std::vector<std::string>::iterator it = cgiEnvVector.begin();
// 		for (int i = 0; it != cgiEnvVector.end(); it++, i++)
// 			env[i] = strdup((*it).c_str());
// 		env[cgiEnvVector.size()] = NULL;
// 		return 0;
// 	}

// 	int initArgv() {
// 		argv = (char **)malloc(sizeof(char *) * 2);
// 		if (!argv)
// 			return 500;
// 		argv[0] = (char *)malloc(sizeof(char) * cgiScriptPath.size() + 1);
// 		if (!argv[0])
// 			return 500;
// 		strcpy(argv[0], cgiScriptPath.c_str());
//  		argv[1] = NULL;
// 		return 0;
// 	}
	
// 	// принимать два параметра - второй с ошибкой
// 	std::string createBodyFromPage(std::string page) {
// 		if (page == ERROR_500) {
// 			perror("ERROR CGI: ");
// 		}
// 		cgiTmpFile.open(page);
// 		std::stringstream bufferCgi;
// 		if (page == ERROR_500) { // костыль
// 			bufferCgi << "Content-Type: text/html\n\n";
// 		}
// 		bufferCgi << cgiTmpFile.rdbuf();
// 		std::string body = bufferCgi.str();
// 		cgiTmpFile.close();
// 		if (page == TMP_CGI)
// 			remove(TMP_CGI);
// 		return body;
// 	}

// 	void createFullPathToScript() {
// 		char cwd[1024];
// 		getcwd(cwd, sizeof(cwd));
// 		std::string cwdStr(cwd);
// 		// this->cgiScriptFullPath = cwdStr + "/" + cgiScriptPath;
// 		this->cgiScriptFullPath = "/Users/cvenkman/Desktop/webserv/www/cgi-bin/" + cgiScriptPath;
// 	}

// public:
// // cgiScriptPath(_cgiScriptPath)
// 	CGI(std::string _requestMethod, std::string _cgiScriptPath) : env(NULL), argv(NULL) {
// 		this->cgiScriptPath = "cgiTest2.bla";
// 		//this->path = const_cast<std::string &>(_path);
// 		/*----обязательно----*/
// 		cgiEnvVector.push_back("GATEWAY_INTERFACE: CGI/1.1");
// 		cgiEnvVector.push_back("SERVER_PROTOCOL: HTTP/1.1");
// 		cgiEnvVector.push_back("SERVER_SOFTWARE: WEBSERV");
// 		cgiEnvVector.push_back("SERVER_NAME: 127.0.0.1");
// 		cgiEnvVector.push_back("SERVER_PORT: 2001");
// 		/*--конец обязательного--*/
// 		cgiEnvVector.push_back("REQUEST_METHOD: " + _requestMethod);
// 		cgiEnvVector.push_back("SCRIPT_NAME: a.out");
// 		cgiEnvVector.push_back("CONTENT_LENGTH: 48");
// 		cgiEnvVector.push_back("CONTENT_TYPE: text/html");
// 		createFullPathToScript();
// 		cgiEnvVector.push_back("PATH_INFO: " + cgiScriptFullPath);
// 		/*
// 		если кидать PATH_INFO то нужно и PATH_TRANSLATED
// 		*/
// 		/*
// 		cgiEnvVector.push_back("REQUEST_LINE");
// 		REQUEST_LINE Содержит строку из запроса протокола HTTP. Например:
// 		GET /cgi-bin/1.cgi HTTP/1.0
// 		*/
// 	}

// 	~CGI() {
// 		if (env) {
// 			for (int i = 0; env[i] != NULL; i++)
// 				free(env[i]);
// 			free(env);
// 		}
// 		if (argv)
// 			free(argv);
// 		//unlink();
// 	}

// 	std::string executeCgiScript() {
// 		if (initEnv() != 0 || initArgv() != 0)
// 			return createBodyFromPage(ERROR_500);

// 		int pip[2];
// 		if (pipe(pip) != 0)
// 			return createBodyFromPage(ERROR_500);
	
// 		pid_t pid = fork();
// 		if (pid < 0)
// 			return createBodyFromPage(ERROR_500);

// 		else if (pid == 0) {
// 			int fd_out = open(TMP_CGI, O_RDWR | O_CREAT, 0777);
// 			if (fd_out == -1)
// 				return createBodyFromPage(ERROR_500);

// 			if (chdir("/Users/cvenkman/Desktop/webserv/www/cgi-bin") == -1)
// 				return createBodyFromPage(ERROR_500);

// 			if (close(pip[1]) == -1)
// 				return createBodyFromPage(ERROR_500);
// 			if (dup2(pip[0], 0) == -1)
// 				return createBodyFromPage(ERROR_500);
// 			// int fd_in = open(cgiScriptPath.c_str(), O_RDONLY);

// 			if (dup2(fd_out, 1) == -1)
// 				return createBodyFromPage(ERROR_500);
// 			// if (close(fd_out) == -1 || close(pip[0]) == -1 ||
// 			// 	close(0) == -1)
// 				// return createBodyFromPage(ERROR_500);
// 			close(pip[0]);
// 			execve(argv[0], argv, env);
			
// 			exit(EXIT_FAILURE);
// 		}
// 		else if (pid > 0) {
// 			int status;
// 			close(pip[0]);
// 			std::string d = createBodyFromPage(TMP_CGI);
// 			// if (write(pip[1], d.c_str(), d.length()) <= 0) {
// 			// 	std::cout << "------\n" << "sax" << "\n-----\n";
// 			// 	return createBodyFromPage(ERROR_500);
// 			// }
// 			close(pip[1]);

// 			waitpid(pid, &status, 0); // проверка waitpid на -1
// 			if (WIFEXITED(status) == 0)
// 				return createBodyFromPage(ERROR_500);
// 		}
// 		std::string body = createBodyFromPage(TMP_CGI);
// 		// std::cout << "------\n" << cgiScriptPath << "\n-----\n";
// 		// std::cout << "------\n" << cgiScriptFullPath << "\n-----\n";
// 		if (strncmp(body.c_str(), "Content-type:", 13) != 0) {
// 			// из-за этого выходит две ошибки ERROR CGI:
// 			std::cout << "ERROR CGI: no Content-type: in cgi script\n";
// 			return createBodyFromPage(ERROR_500);
// 		}
// 		return body;
// 	}
// };
