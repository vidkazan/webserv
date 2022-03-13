#include "main.hpp"

void    printEnv(char** env){
    while(*env){
        std::cout << *env << "\n";
        env++;
    }
}

std::string CGI::error500ContentType = "Content-Type: text/html";
std::string CGI::error500Body = "<html>\n<h1>500 Server Error</h1>\n</html>";
int CGI::error500BodySize = 40;


/**
**	@brief	Construct a new CGI object
**	@param	_requestMethod		POST/GET
**	@param	_cgiScriptPath		cgi script path
*/
CGI::CGI(std::string _requestMethod, std::string _cgiScriptPath, std::string cgiOutputFileName, std::string cgiInputFileName)
		: env(NULL), argv(NULL), cgiScriptPath(_cgiScriptPath), _cgiOutputFileName(cgiOutputFileName), _cgiInputFileName(cgiInputFileName) {
	/*----обязательно----*/
	cgiEnvVector.push_back("GATEWAY_INTERFACE=CGI/1.1");
	cgiEnvVector.push_back("SERVER_PROTOCOL=HTTP/1.1");
	cgiEnvVector.push_back("SERVER_SOFTWARE=WEBSERV");
	cgiEnvVector.push_back("SERVER_NAME=127.0.0.1");
	cgiEnvVector.push_back("SERVER_PORT=2001");
	cgiEnvVector.push_back("REQUEST_METHOD=" + _requestMethod);
	/*--конец обязательного--*/
	cgiEnvVector.push_back("SCRIPT_NAME=a.out");
//	if (_requestMethod == "POST") {
//		cgiEnvVector.push_back("CONTENT_LENGTH=48");
//		cgiEnvVector.push_back("CONTENT_TYPE=text/html");
//	}
	/*
		если кидать PATH_INFO то нужно и PATH_TRANSLATED
	*/
	/*
	cgiEnvVector.push_back("REQUEST_LINE");
	REQUEST_LINE Содержит строку из запроса протокола HTTP. Например:
	GET /cgi-bin/1.cgi HTTP/1.0
	*/
}

/**
**	@brief	create full path to cgi script
*/
void CGI::createFullPathToScript() {
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		throw CreateFullPathException("getcwd");
	std::string cwdStr(cwd);
	this->cgiScriptFullPath = cwdStr + "/" + cgiScriptPath;
}

/**
**	@brief	Destroy the CGI object free env and argv
*/
CGI::~CGI() {
	for (int i = 0; env[i] != NULL; i++)
		free(env[i]);
	free(env);
	free(argv[0]);
	free(argv);
}

/**
**	@brief	execute cgi script via fork & execve
**	@return	std::string	script output in html (without "Content-type:")
*/
void CGI::executeCgiScript() {
	createFullPathToScript(); // здесь, а не в конструкторе, т.к. выбрасывает исключение
	cgiEnvVector.push_back("PATH_INFO=" + cgiScriptFullPath);

	initEnv();
	initArgv();

//	int pip[2];
//	if (pipe(pip) != 0)
//		throw CreateFullPathException("cannot pipe()");

	pid_t pid = fork();
//    printEnv(env);
	if (pid < 0)
		throw CreateFullPathException("cannot fork()");
	else if (pid == 0) {
		// chdir
//		if (close(pip[1]) == -1)
//			throw CreateFullPathException("cannot close() pip[1]");
        int fd_in = open(this->getCgiInputFileName().c_str(), O_RDONLY, 0777);
        if (fd_in == -1)
            throw CreateFullPathException("cannot open() input file");
		int fd_out = open(this->getCgiOutputFileName().c_str(), O_RDWR | O_CREAT | O_TRUNC, 0777);
        if (fd_out == -1)
			throw CreateFullPathException("cannot open() output file");
        if (dup2(fd_in, 0) == -1 || dup2(fd_out, 1) == -1)
            throw CreateFullPathException("cannot dup2()");
        if (close(fd_out) == -1 || close(fd_in) == -1 ||
            close(0) == -1)
            throw CreateFullPathException("cannot close()");
//		if (dup2(pip[0], 0) == -1 || dup2(fd_out, 1) == -1)
//			throw CreateFullPathException("cannot dup2()");
//		if (close(fd_out) == -1 || close(pip[0]) == -1 ||
//			close(0) == -1)
//			throw CreateFullPathException("cannot close()");
//        std::cout << argv[0] <<"\n";
		if(execve(argv[0], argv, env))
            strerror(errno);
		exit(EXIT_FAILURE);
	}
	else if (pid > 0) {
		int status;
//		close(pip[0]);
//		close(pip[1]);
		waitpid(pid, &status, 0);
		if (WIFEXITED(status) == 0)
			throw CreateFullPathException("process failed");
	}
	createBodyFromPage(this->getCgiOutputFileName());
	initContentType();
}

/**
**	@brief	init cgi environment for execve
*/
void CGI::initEnv() {
	env = (char **)malloc(sizeof(char *) * cgiEnvVector.size() + 1);
	if (!env)
		throw CreateFullPathException("malloc for env");

	std::vector<std::string>::iterator it = cgiEnvVector.begin();
	for (int i = 0; it != cgiEnvVector.end(); it++, i++) {
		env[i] = strdup((*it).c_str());
		if (!env[i])
			throw CreateFullPathException("malloc for env[i]");
	}
	env[cgiEnvVector.size()] = NULL;
}

/**
**	@brief	init argv for execve
*/
void CGI::initArgv() {
	argv = (char **)malloc(sizeof(char *) * 2);
	if (!argv)
		throw CreateFullPathException("malloc for argv");

	argv[0] = (char *)malloc(sizeof(char) * cgiScriptPath.size() + 1);
	if (!argv[0])
		throw CreateFullPathException("malloc for argv[0]");
	strcpy(argv[0], cgiScriptPath.c_str());
	argv[1] = NULL;
}

// удалить page?
/**
**	@brief	open file with page name, get his buffer
** 			and delete first string with content type
**	@param	page		file name
*/
void CGI::createBodyFromPage(std::string page) {
	// if (page == ERROR_500) {
	// 	this->contentTypeStr = "Content-Type: text/html";
	// 	perror("ERROR CGI: ");
	// }
	std::fstream cgiTmpFile;
	cgiTmpFile.open(page);
	std::stringstream bufferCgi;
	// if (page == ERROR_500) { // костыль
	// 	bufferCgi << "Content-Type: text/html\n\n";
	// }
	bufferCgi << cgiTmpFile.rdbuf();
	bodyAndHeader = bufferCgi.str();
	std::string bodyWithoutHeader(bodyAndHeader,
		bodyAndHeader.find_first_of("\n") + 2, bodyAndHeader.length());
	this->body = bodyWithoutHeader;
	cgiTmpFile.close();
	// if (page == TMP_CGI)
//		remove(page.c_str());
	// return bodyWithoutHeader;
}

/**
**	@brief	get first string with content type from body
**			(throw if the body does not contain a line with or no \\n)
*/
void CGI::initContentType() {
	if (strncmp(bodyAndHeader.c_str(), "Content-type:", 13) != 0)
		throw CreateFullPathException("no Content-type: in cgi script");

	std::string tmp(bodyAndHeader, 0, bodyAndHeader.find_first_of("\n"));
	this->contentTypeStr = tmp;

	if (bodyAndHeader[contentTypeStr.length() + 1] != '\n')
		throw CreateFullPathException("no \\n after header");
}

std::string CGI::getContentTypeStr() {
	return this->contentTypeStr;
}

std::string CGI::getBody() {
	return this->body;
}