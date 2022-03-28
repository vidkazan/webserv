//
// Created by Feyd Rodney on 2/20/22.
//

#include "serverConfig.hpp"


/*
 * Constructor and destructor
 */

ServerConfig::ServerConfig(string const & raw) :
		IParse() {
	this->_raw = raw;
	this->_nulling();
	this->_parse();
//	this->_printConfigurations();
}

ServerConfig::~ServerConfig() {
	if (!this->locations.empty()) {
		vector<LocationConfig *>::iterator b = this->locations.begin();
		vector<LocationConfig *>::iterator e = this->locations.end();
		while (b != e) {
			delete *b;
//			this->locations.erase(b);
			b++;
		}
	}
	if (this->listen != NULL)
		delete this->listen;
}

/*
 * service methods: _nulling and _idPole
 */

void ServerConfig::_nulling() {
	this->listen = NULL;
	this->client_body_buffer_size = -1;
	this->root = "./";

}

void ServerConfig::_idPole(string pole) {

	void    (ServerConfig::*member[8])() = {
		&ServerConfig::_setServerName,
		&ServerConfig::_setCBBS,
		&ServerConfig::_setIndex,
		&ServerConfig::_setListen,
		&ServerConfig::_setLocation,
		&ServerConfig::_setAutoindex,
		&ServerConfig::_setRoot,
		&ServerConfig::_setErrPage
	};
	int num = ("server_name" == pole) * 1 +
			("client_body_buffer_size" == pole) * 2 +
			("index" == pole) * 3 +
			("listen" == pole) * 4 +
			("location" == pole) * 5 +
			("autoindex" == pole) * 6 +
			("root" == pole) * 7 +
			("error_page" == pole) * 8;
	if (num)
		(this->*member[num - 1])();
	else
		throw std::runtime_error (string(ERR_INTERPRET) + ": '" + pole + "'");

}

/*
 * setters of server name, client body buffer size,
 * listen and locations
 */

void ServerConfig::_setServerName() {
	if (!this->server_name.empty())
		throw std::runtime_error (ERR_DOUBLE_ASSIGMENT);

	this->_rawErase("server_name ");

	this->server_name = this->_getSingleValue();
}

void ServerConfig::_setListen() {
	this->_rawErase("listen ");

	if (this->listen != NULL) {
		string port = this->_getSingleValue();
		if (this->_validPort(port) == false)
			throw std::runtime_error(ERR_INVALID);
		return;
	} else {
		state s = this->_validIPPort();
		if (s == IP_PORT)
			return;
		else if (s == IP) {
			if (this->_raw.empty())
				return;
			string port = this->_getSingleValue();
			if (this->_validPort(port) == false) {
				if (!this->_raw.empty())
					this->_raw = port + " " + this->_raw;
			}
			return;
		} else
			throw std::runtime_error(ERR_INVALID);
	}
}

void ServerConfig::_setLocation() {

	/*
	 * strict format configurations locations:
	 * 'location_'directory'_{configuration_parameter}'
	 *
	 * In this method we cut path of locations, cut locations body (rawLocation), trim '{...}'
	 * and put it in the constructor of class LocationConfig
	 */

	this->_rawErase("location ");
	string directory = this->_getSingleValue();

	if (*(this->_raw.begin()) == '{')
		this->_raw.erase(0, 1);
	else
		throw std::runtime_error(ERR_INTERPRET); // TODO slabo

	size_t pos = this->_raw.find('}');
	string rawLocation = this->_raw.substr(0, pos);

	if (rawLocation.length() != this->_raw.length()+1)
		this->_raw.erase(0, pos + 2); // locations is not last(+" ")
	else
		this->_raw.erase(0, pos + 1); // locations is last

	LocationConfig * locations = new LocationConfig(rawLocation, directory);
	this->locations.push_back(locations);
}

void ServerConfig::_printConfigurations() {
	cout << "======================COUT_CONFIGURATIONS======================\n" ;
	cout << "rawip: " << this->listen->rawIp << endl;
	cout << "serv name: " << this->server_name << endl;
	cout << "client_body_buffer_size: " << this->client_body_buffer_size << endl;
	cout << "index: " << this->index << endl;
	cout << "autoindex: ";
	if (this->autoindex == true)
		cout << "on";
	else
		cout << "off";
	cout << endl;
	if (this->listen) {
		cout << "listen ip : " << this->listen->rawIp << endl;
		if (!this->listen->port.empty()) {
			vector<short>::iterator b = this->listen->port.begin();
			vector<short>::iterator e = this->listen->port.end();
			while (b!=e){
				cout << "listen port: " << *b << endl;
				b++;
			}
		}
	}
	if (!this->root.empty())
		cout << "serv root : " << this->root << endl;
	if (!this->error_pages.empty()) {
		cout << "location err_page:" << endl;
		for (map<int, string>::iterator it = this->error_pages.begin();
			 it != this->error_pages.end(); it++)
			cout << "code: " << it->first << " file: " << it->second << endl;
	}
	if (!this->locations.empty()) {
		vector<LocationConfig *>::iterator b = this->locations.begin();
		vector<LocationConfig *>::iterator e = this->locations.end();
		while (b != e) {
			if (!(*b)->allow_methods.empty())
				cout << "location methods:\t" << (*b)->allow_methods << endl;
			if (!(*b)->root.empty())
				cout << "location root:\t" << (*b)->root << endl;
			if (!(*b)->index.empty())
				cout << "location index:\t" << (*b)->index << endl;
			if (!(*b)->cgi_path.empty())
				cout << "location cgi_path:\t" << (*b)->cgi_path << endl;
			if (!(*b)->cgi_extension.empty())
				cout << "location cgi_extension:\t" << (*b)->cgi_extension << endl;
			cout << "location autoindex:\t";
			if ((*b)->autoindex == true)
				cout << "on";
			else
				cout << "off ";
			cout << endl;
			if (!(*b)->allow_methods.empty()) {
				cout << "location allow_meth:\t" << (*b)->allow_methods << endl;
			}
			if (!(*b)->error_pages.empty()) {
				cout << "location err_page:" << endl;
				for (map<int, string>::iterator it = (*b)->error_pages.begin();
					 it != (*b)->error_pages.end(); it++)
					cout << "code: " << it->first << " file: " << it->second << endl;
			}
			b++;
		}
	}
}


