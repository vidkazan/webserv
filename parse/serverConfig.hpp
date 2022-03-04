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


#include <filesystem>

#include "IParse.hpp"

enum state {IP, IP_PORT, NOTHING};

class IParse;
class LocationConfig;

typedef struct s_listen {
	uint8_t ip[4]; // TODO m?
	char *rawIp;
	vector<short>	port;
} t_listen;

class ServerConfig : public IParse {
public:

	virtual ~ServerConfig();
	ServerConfig() {};
	ServerConfig(string const & raw);

	string 						server_name;
	vector<LocationConfig *>	locations; // vector
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

	void _nulling();
};

#endif //WEBSERV_SERVERCONFIG_HPP
