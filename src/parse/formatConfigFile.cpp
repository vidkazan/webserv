//
// Created by Feyd Rodney on 2/1/22.
//

#include "formatConfigFile.hpp"
#include <strstream>

formatConfigFile::formatConfigFile(int argc, char **argv) {
	if (argc != 2)
		throw std::runtime_error (ERR_ARG);

	this->_ftostr(argv[1]);
	this->_formatFile();
	this->_separateStringServers();
	this->_eraseExternal();
}

formatConfigFile::~formatConfigFile() {}

void formatConfigFile::_formatFile() {
	// erase \n \t and #*\n
	// + brackets closing check
	size_t	pos;

	while ((pos = this->_fileContent.find('#')) != string::npos) // check!
		this->_fileContent.erase(pos, (this->_fileContent.find('\n', pos) - pos));
	while ((pos = this->_fileContent.find("server ")) != string::npos)
			this->_fileContent.erase(pos + 6, 1);
	while ((pos = this->_fileContent.find('\t')) != string::npos)
		this->_fileContent.replace (pos, 1, " ");
	while ((pos = this->_fileContent.find("  ")) != string::npos ||
		   (pos = this->_fileContent.find( '\n')) != string::npos)
		this->_fileContent.erase(pos, 1);
	while ((pos = this->_fileContent.find("{ ")) != string::npos)
		this->_fileContent.erase(pos + 1, 1);
	while ((pos = this->_fileContent.find(" }")) != string::npos)
		this->_fileContent.erase(pos, 1);
	this->_bracketsCheck();

}

void formatConfigFile::_bracketsCheck() {
	int	countBr = 0;

	string::iterator fB(this->_fileContent.begin());
	string::iterator fE(this->_fileContent.end());

	while (fB != fE) {
		if (*fB == '{') {
			countBr++;
		}
		else if (*fB == '}') {
			countBr--;
			if (countBr < 0)
				throw std::runtime_error("error with } ");
		}
		fB++;
	}
	if (countBr != 0)
		throw std::runtime_error("wrong counting br result");
}

void formatConfigFile::_separateStringServers() {
	string tmp;
	size_t pos;
	size_t nextPos;

	while ((pos = this->_fileContent.find("server{")) != string::npos){
		if (pos != 0)
			throw std::runtime_error (ERR_PARSE);
		nextPos = this->_fileContent.find ("server{", pos + 6);
		if (nextPos == string::npos) {
			tmp = this->_fileContent.substr(pos);
			this->_fileContent.erase(pos);
		}
		else {
			tmp = this->_fileContent.substr(pos, nextPos);
			this->_fileContent.erase(pos, nextPos);
		}
		this->_stringServers.push_back(tmp);
	}
	if (!this->_fileContent.empty())
		throw std::runtime_error(ERR_PARSE);
}

vector<string> formatConfigFile::getStringServers() {
	return this->_stringServers;
}

void formatConfigFile::_eraseExternal() {
	vector<string>::iterator b (this->_stringServers.begin());
	vector<string>::iterator e (this->_stringServers.end());

	while (b != e) {
		size_t sL = b->length();
		if (b->find('{') != 6
			|| b->substr(0, 6) != "server"
			|| b->substr(sL - 1) != "}") {
			throw std::runtime_error(ERR_PARSE);
		}
		b->erase(0, 7);
		b->erase(b->length() - 1, 1);
		b++;
	}
}

void	formatConfigFile::_ftostr (string const & fileName) {
	string	conf = ".conf";
	size_t n;
	if ( (n = fileName.rfind(conf)) == string::npos ||
		 n + conf.length() != fileName.length() )
		throw std::runtime_error(ERR_ARG);

	std::ifstream t(fileName.c_str());

	if (t.fail())
		throw std::runtime_error(ERR_ARG);

	string line;
	while(t){
		std::getline(t, line);
		this->_fileContent += line + "\n";
	}
	t.close();

}
