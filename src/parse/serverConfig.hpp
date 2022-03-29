//
// Created by nastya on 18.02.2022.
//

#ifndef WEBSERV_SERVERCONFIG_HPP
#define WEBSERV_SERVERCONFIG_HPP

#include <iostream>
#include <string>
#include <fstream>
#include "formatConfigFile.hpp"
#include "LocationConfig.hpp"
#include "IParse.hpp"

enum state {IP, IP_PORT, NOTHING};

class IParse;
class LocationConfig;

typedef struct s_listen {
	string rawIp;
	vector<short>	port;
} t_listen;

class ServerConfig : public IParse {
public:

	virtual ~ServerConfig();
	ServerConfig() {};
	ServerConfig(string const & raw);

	string 						server_name;
	vector<LocationConfig *>	locations;
	t_listen *					listen;



private:

	void _setServerName();

	void _setListen();

	state _validIPPort();

	void _makeListen();

	bool _validIP(string IP);

	bool _validPort(string port);

	void _setLocation ();

	void _printConfigurations();

	void _idPole(string basicString);

};

#endif //WEBSERV_SERVERCONFIG_HPP
