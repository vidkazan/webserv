#include "../main.hpp"

std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

void    printEnv(char** env){
    while(*env){
        std::cout << *env << "\n";
        env++;
    }
}

std::string CGI::error500ContentType = "Content-Type: text/html";
std::string CGI::error500Body = "<html>\n<h1>500 Server Error</h1>\n</html>";
int CGI::error500BodySize = CGI::error500Body.size();

/**
**	@brief	Construct a new CGI object
**	@param	_requestMethod		POST/GET
**	@param	_cgiScriptPath		cgi script path
*/
CGI::CGI(std::string _requestMethod, std::string _cgiScriptPath,
			std::string cgiOutputFileName, std::string cgiInputFileName, 
			std::string ip, short _port)
		: scriptPath(_cgiScriptPath),env(NULL), argv(NULL),
		_cgiOutputFileName(cgiOutputFileName), _cgiInputFileName(cgiInputFileName),
		ip(ip), port(std::to_string(_port)) {
	/*----обязательно----*/
	cgiEnvVector.push_back("GATEWAY_INTERFACE=CGI/1.1");
	cgiEnvVector.push_back("SERVER_PROTOCOL=HTTP/1.1");
	cgiEnvVector.push_back("SERVER_SOFTWARE=WEBSERV"); //FIXME
	cgiEnvVector.push_back("SERVER_NAME=" + ip);
	cgiEnvVector.push_back("SERVER_PORT=" + port);
	cgiEnvVector.push_back("REQUEST_METHOD=" + _requestMethod);
    cgiEnvVector.push_back("HTTP_X_SECRET_HEADER_FOR_TEST=1");
	/*--конец обязательного--*/
	scriptFileName = scriptPath.substr(scriptPath.find_last_of('/') + 1, scriptPath.size());
    scriptPath = scriptPath.substr(0,scriptPath.find_last_of('/'));
}

/**
**	@brief	create full path to cgi script
*/
void CGI::createFullPathToScript() {
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		throw StandartFunctionsException("getcwd");
	std::string cwdStr(cwd);
    this->scriptFullPath = cwdStr + "/" + scriptPath + "/" + scriptFileName;
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

bool CGI::checkScriptRights() { // изменить логик bool
	if (access(scriptFullPath.c_str(), F_OK) == -1) {
		/* проверяем на существование */
		std::fstream inputFile;
		std::cerr << "ERROR IN CGI: cgi script not found\n";
		cgiBufResp = "HTTP/1.1 404 Not found\n";
		inputFile.open("www/404.html", std::ios::in); // FIXME прикрутить путь из конфига
		std::stringstream buffer;
		buffer << inputFile.rdbuf();
		body = buffer.str();
		return false;
	}
	if (access(scriptFullPath.c_str(), X_OK) == -1) {
		/* проверяем на право выполнения */
        std::cout << scriptFullPath << "\n";
		throw StandartFunctionsException("no rights");
	}
	return true;
}

/**
**	@brief	execute cgi script via fork & execve
**	@return	std::string	script output in html (without "Content-type:")
*/
void CGI::executeCgiScript() {
	createFullPathToScript();
	/* здесь, а не в конструкторе, т.к. выбрасывает исключение */
    std::string path_info = "PATH_INFO=" + scriptFullPath.substr(0,scriptFullPath.find_last_of('/') + 1);
	cgiEnvVector.push_back(path_info);

	initEnv();
	initArgv();
	if (checkScriptRights() == false)
		return;
	pid_t pid = fork();
	if (pid == 0) {
        int fd_in = open(getCgiInputFileName().c_str(), O_RDONLY, 0777);
        if (fd_in == -1)
            throw StandartFunctionsException("cannot open() input file");
		int fd_out = open(getCgiOutputFileName().c_str(), O_RDWR | O_CREAT | O_TRUNC, 0777);
        if (fd_out == -1)
			throw StandartFunctionsException("cannot open() output file");
        if ((chdir(scriptPath.c_str())) == -1)
			throw StandartFunctionsException("cannot change directory");
        if (dup2(fd_in, 0) == -1 || dup2(fd_out, 1) == -1)
            throw StandartFunctionsException("cannot dup2()");
        if (close(fd_out) == -1 || close(fd_in) == -1)
            throw StandartFunctionsException("cannot close()");
        if (execve(argv[0], argv, env))
			strerror(errno);
		exit(EXIT_FAILURE);
	}
	else if (pid > 0) {
		int status;
		waitpid(pid, &status, 0);
		if (WIFEXITED(status) == 0) {
            throw StandartFunctionsException("process failed");
        }
	}
    createBodyFromFile();
	cgiBufResp = "HTTP/1.1 200 OK\r\n";
    initContentType(); //cgi this
	this->contentTypeStr = this->getContentTypeStr(); //cgi this
}

/**
**	@brief	init cgi environment for execve
*/
void CGI::initEnv() {
	env = (char **)malloc(sizeof(char *) * cgiEnvVector.size() + 1);
	if (!env)
		throw StandartFunctionsException("malloc for env");

	std::vector<std::string>::iterator it = cgiEnvVector.begin();
	for (int i = 0; it != cgiEnvVector.end(); it++, i++) {
		env[i] = strdup((*it).c_str());
		if (!env[i])
			throw StandartFunctionsException("malloc for env[i]");
	}
	env[cgiEnvVector.size()] = NULL;
}

/**
**	@brief	init argv for execve
*/
void CGI::initArgv() {
	argv = (char **)malloc(sizeof(char *) * 2);
	if (!argv)
		throw StandartFunctionsException("malloc for argv");

	argv[0] = (char *)malloc(sizeof(char) * scriptFileName.size() + 1);
	if (!argv[0])
		throw StandartFunctionsException("malloc for argv[0]");
	strcpy(argv[0], scriptFileName.c_str());
	argv[1] = NULL;
}

/**
**	@brief	open file with page name, get his buffer
** 			and delete first string with content type
**	@param	page		file name
*/
void CGI::createBodyFromFile()
{
	std::fstream cgiTmpFile;
	cgiTmpFile.open(getCgiOutputFileName());
    if(!cgiTmpFile.is_open())
        throw StandartFunctionsException("createBodyFromFile: error open output file");
	std::stringstream bufferCgi;
	bufferCgi << cgiTmpFile.rdbuf();
	bodyAndHeader = bufferCgi.str();

    size_t pos = bodyAndHeader.find("\r\n\r\n");
    if(pos != std::string::npos)
        pos += 4;
    else
        pos=0;
    this->body = bodyAndHeader.substr(pos,bodyAndHeader.size() - pos);

	std::string bodyWithoutHeader(this->body, //cgi this
		this->body.find_first_of("\n") + 2, this->body.length()); //cgi this
	this->body = bodyWithoutHeader; //cgi this
	
	cgiTmpFile.close();
}
/**
**	@brief	get first string with content type from body
**			(throw if the body does not contain a line with or no \\n)
*/
void CGI::initContentType() { // нужна ли проврека на Content-type

	// if (bodyAndHeader.find("Content-type:") != std::string::npos)
	if (strncmp(bodyAndHeader.c_str(), "Content-type:", 13) != 0) //cgi this
		throw StandartFunctionsException("no Content-type: in cgi script");

	std::string tmp(bodyAndHeader, 0, bodyAndHeader.find_first_of("\n"));
	this->contentTypeStr = tmp;

	if (bodyAndHeader[contentTypeStr.length() + 1] != '\n')
		throw StandartFunctionsException("no \\n after header");
}

std::string CGI::getContentTypeStr() {
	return this->contentTypeStr;
}

std::string CGI::getBody() {
	return this->body;
}

std::string CGI::getBufResp() {
	return this->cgiBufResp;
}