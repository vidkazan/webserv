#include "formatConfigFile.hpp"
#include "errorCodes.hpp"
#include <strstream>

string	ftostr (string const & fileName) {
	string	conf = ".conf";
	size_t n;
	if ( (n = fileName.rfind(conf)) == string::npos ||
	n + conf.length() != fileName.length() )
		throw std::runtime_error(ERR_ARG);

	std::ifstream t(fileName.c_str());

	if (t.fail())
		throw std::runtime_error(ERR_ARG);

	strstream buffer;

	buffer << t.rdbuf();
	t.close();
	return buffer.str();

}
