#pragma once

#define ERROR_500 "www/500.html"
#define TMP_CGI "tmp_cgi_file.$"
#define CGI_ERROR "ERROR IN CGI: "

class CGI {
private:
	std::string cgiScriptPath;
	std::string cgiScriptFullPath;
	std::vector<std::string> cgiEnvVector;
	char **env;
	char **argv;
	// std::fstream cgiTmpFile;
	std::string contentTypeStr;
	std::string bodyAndHeader;
	std::string body;

	void initEnv();
	void initArgv();
	void initContentType();

	void createBodyFromPage(std::string page);
	void createFullPathToScript();

public:

	static std::string error500Body;
	static std::string error500ContentType;
	static int error500BodySize;

	class CreateFullPathException : public std::exception {
		private:
			std::string error;
		public:
			CreateFullPathException(std::string const& error) : error(error) {};
			const char* what() const throw() {return error.c_str(); };
			~CreateFullPathException() throw() {};
	};

	CGI(std::string _requestMethod, std::string _cgiScriptPath);
	~CGI();
	void executeCgiScript();
	std::string getContentTypeStr();
	std::string getBody();
};
