#pragma once

#include "../main.hpp"

#define ERROR_500 "www/500.html"

// class VirtualServerConfig;
class CGI {
private:
	std::string scriptFileName;
	std::string scriptPath;
	std::string scriptFullPath;
	std::string cgiBufResp;
	std::vector<std::string> cgiEnvVector;
	char **env;
	char **argv;
	std::string contentTypeStr;
	std::string bodyAndHeader;
	std::string body;

	VirtualServerConfig serverConfig;

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


	CGI(std::string _requestMethod, std::string _cgiScriptPath, std::string cgiInputFileName,
		std::string cgiOutputFileName, VirtualServerConfig serverConfig);

	~CGI();
	void executeCgiScript();
	bool checkScriptRights();
	std::string getContentTypeStr();
	std::string getBody();
	std::string getBufResp();

    std::string getCgiInputFileName(){return _cgiInputFileName;}
    std::string getCgiOutputFileName(){return _cgiOutputFileName;}
    void setCgiInputFileName(std::string name){_cgiInputFileName = name;};
    void setCgiOutputFileName(std::string name){_cgiOutputFileName = name;};
};
