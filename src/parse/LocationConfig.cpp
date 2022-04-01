//
// Created by Feyd Rodney on 2/28/22.
//

#include "LocationConfig.hpp"

LocationConfig::LocationConfig(string const & raw, string & dir) : IParse() {
	this->_raw = raw;
	this->autoindex = OFF;
	this->client_body_buffer_size = -1;
	this->_setName(dir);
	this->_setDefaultErrorPages();
	this->_parse();
	if (!this->redirect.empty()) {
		if (!this->root.empty())
			this->root.erase();
		if (!this->cgi_extension.empty())
			this->cgi_extension.erase();
		if (!this->cgi_path.empty())
			this->cgi_path.erase();
		if (!this->index.empty())
			this->index.erase();
	}
}
void LocationConfig::_idPole(string pole) {
	void    (LocationConfig::*member[9])() = {
			&LocationConfig::_setRoot,
			&LocationConfig::_setIndex,
			&LocationConfig::_setCGIPath,
			&LocationConfig::_setAutoindex,
			&LocationConfig::_setMetods,
			&LocationConfig::_setCGIExtension,
			&LocationConfig::_setCBBS,
			&LocationConfig::_setErrPage,
			&LocationConfig::_setRedir
	};
	int num =	("root" == pole) * 1 +
				("index" == pole) * 2 +
				("cgi_path" == pole) * 3 +
				("autoindex" == pole) * 4 +
				("method" == pole) * 5 +
				("cgi_extension" == pole) * 6 +
				("client_body_buffer_size" == pole) * 7 +
				("error_page" == pole) * 8 +
				("redirect" == pole) * 9;
	if (num) {
		(this->*member[num - 1])();
	}
	else
		throw std::runtime_error (string(ERR_INTERPRET) + ": '" + pole + "'");
}

void LocationConfig::_setName(string & value) {
	if (*value.begin() != '/')
		throw std::runtime_error (string(ERR_INTERPRET) + ": '" + value + "'");
	this->name = value;
}

void LocationConfig::_setCGIPath() {
	this->_rawErase("cgi_path ");
	string directory = this->_getSingleValue();
	this->_checkFile(directory);
	this->cgi_path = directory;
}

void LocationConfig::_setMetods() {
	this->_rawErase("method ");

	string value;

	value = this->_getSingleValue();
	if (value == "GET" ||
		value == "POST" ||
		value == "DELETE" ||
        value == "HEAD" ||
		value == "PUT")
		this->allow_methods += value;
	while (42) {
		if (this->_raw.empty())
			return;
		value = this->_getSingleValue();
		if (value == "GET" ||
				value == "POST" ||
				value == "DELETE" ||
                value == "HEAD" ||
				value == "PUT") {
			this->allow_methods += " " + value;
		}
		else {
			if (this->_raw.empty())
				throw std::runtime_error (string(ERR_INTERPRET) + ": '" + value + "'");
			this->_raw = value + " " + this->_raw;
			break;
		}
	}
}

void LocationConfig::_setCGIExtension() {
	this->_rawErase("cgi_extension ");
	this->cgi_extension = this->_getSingleValue();
	std::string tmp(cgi_extension, 1, cgi_extension.size() - 1);
	this->cgi_extension = tmp;
}

void LocationConfig::_setRedir() {

	this->_rawErase("redirect ");
	if (!this->_raw.empty())
		this->redirect = this->_getSingleValue();
	else throw std::runtime_error (ERR_PARSE);
}
