//
// Created by Feyd Rodney on 2/28/22.
//

#ifndef WEBSERV_LOCATIONCONFIG_HPP
#define WEBSERV_LOCATIONCONFIG_HPP

#include <vector>
#include "IParse.hpp"

using namespace std;

class IParse;

class LocationConfig  : public IParse {
public:
	string 			name;
	string			cgi_path;
	string			cgi_extension;
	string			allow_methods;
	string 			redirect;

	LocationConfig(string const & raw, string & dir);
	~LocationConfig() {}
private:
	void _idPole(string basicString);

	void _setName(string & value);

	void _setCGIPath();

	void _setMetods();

	void _setCGIExtension();

	void _setRedir();
};


#endif //WEBSERV_LOCATIONCONFIG_HPP
