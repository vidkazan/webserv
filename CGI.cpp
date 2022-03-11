#include "CGI.hpp"

// std::string CGI::error500Body = "Content-Type: text/html\n\n\
// 			<html>\n<h1>500 Server Error1</h1>\n</html>\n";
// int CGI::error500BodySize = 45;

bool CGI::initEnv() {
	env = (char **)malloc(sizeof(char *) * cgiEnvVector.size() + 1);
	if (!env)
		return false;

	std::vector<std::string>::iterator it = cgiEnvVector.begin();
	for (int i = 0; it != cgiEnvVector.end(); it++, i++)
		env[i] = strdup((*it).c_str());
	env[cgiEnvVector.size()] = NULL;
	return true;
}