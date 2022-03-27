//
// Created by Feyd Rodney on 2/27/22.
//

#include "serverConfig.hpp"

state ServerConfig::_validIPPort() {
	size_t pos;
	string value = this->_getSingleValue();
	string ip;
	string port;


	if ((pos = value.find(":")) != string::npos) {
		this->_makeListen();
		ip = value.substr(0, pos);
		value.erase(0, pos + 1);
		port = value;

		if (this->_validIP(ip) == false || this->_validPort(port) == false) {
			throw std::runtime_error(ERR_INVALID);
		}

		return IP_PORT;
	} else {
		this->_makeListen();
		if (this->_validIP(value) != false)
			return IP;
		delete this->listen;
		return NOTHING;
	}
}

void ServerConfig::_makeListen() {
	if (this->listen != NULL)
		throw std::runtime_error (ERR_DOUBLE_ASSIGMENT);
	this->listen = new t_listen;
}

bool ServerConfig::_validIP(string ip) {
	if (ip == "localhost") {
		this->listen->rawIp = (char *)"127.0.0.1";
		return true;
	}

	int i = 0;
	size_t		pos;
	string		rawOktet;
	int			oktet;

	this->listen->rawIp = ip;
	while ((pos = ip.find(".")) != string::npos) {
		rawOktet = ip.substr(0,pos);
		ip.erase(0, pos+1);
		oktet = stoi(rawOktet, &pos);
		if (pos != rawOktet.length() || oktet < 0 || oktet > 255) {
			this->listen->rawIp.erase();
			return false;
		}
		i++;
	}
	if (i != 3)
		return false;
	rawOktet = ip;
	oktet = stoi(rawOktet, &pos);
	if (pos != rawOktet.length() || oktet < 0 || oktet > 255)
		return false;
	return true;
}

bool ServerConfig::_validPort(string port) {
	size_t pos;
	if (!this->listen)
		throw std::runtime_error(ERR_INTERPRET);
	int p;
	try {p = stoi(port, &pos);}
	catch (exception & ex) {
		return false;
	}

	if (pos != port.length())
		return false;
	if (p < 0 || p > 65535) {
		throw std::runtime_error(ERR_INVALID);
	}
	this->listen->port.push_back(static_cast<short>(p));
	return true;
}

