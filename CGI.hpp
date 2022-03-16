#pragma once

#include "main.hpp"

#define ERROR_500 "www/500.html"
#define TMP_CGI "tmp_cgi_file.$"
#define CGI_ERROR "ERROR IN CGI: "

class CGI {
private:
	std::string scriptFileName;
	std::string scriptPath;
	std::string scriptFullPath;
	std::string cgiBufResp;
	std::vector<std::string> cgiEnvVector;
	char **env;
	char **argv;
	// std::fstream cgiTmpFile;
	std::string contentTypeStr;
	std::string bodyAndHeader;
	std::string body;

    std::string _cgiOutputFileName;
    std::string _cgiInputFileName;

	void initEnv();
	void initArgv();
	void initContentType();

	void createBodyFromFile();
	void createFullPathToScript();

public:

	static std::string error500Body;
	static std::string error500ContentType;
	static int error500BodySize;

	class StandartFunctionsException : public std::exception {
		private:
			std::string error;
		public:
			StandartFunctionsException(std::string const& error) : error(error) {};
			const char* what() const throw() {return error.c_str(); };
			~StandartFunctionsException() throw() {};
	};


	CGI(std::string _requestMethod, std::string _cgiScriptPath, std::string cgiInputFileName, std::string cgiOutputFileName);

	~CGI();
	void executeCgiScript();
	void checkScriptRights();
	std::string getContentTypeStr();
	std::string getBody();
	std::string getBufResp();

    std::string getCgiInputFileName(){return _cgiInputFileName;}
    std::string getCgiOutputFileName(){return _cgiOutputFileName;}
    void setCgiInputFileName(std::string name){_cgiInputFileName = name;};
    void setCgiOutputFileName(std::string name){_cgiOutputFileName = name;};
};
