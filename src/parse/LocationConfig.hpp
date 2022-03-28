//
// Created by Feyd Rodney on 2/28/22.
//

#ifndef WEBSERV_LOCATIONCONFIG_HPP
#define WEBSERV_LOCATIONCONFIG_HPP

#include <vector>
#include "IParse.hpp"

using namespace std;

class IParse;

//enum methods {GET, POST, DELETE, PUT};

class LocationConfig  : public IParse {
public:
	string 			name;
	string			cgi_path; // TODO vector<string>? def:NULL
	string			cgi_extension;
	vector<string>	allow_methods; // yay or nay?

	LocationConfig(string const & raw, string & dir);
	~LocationConfig() {}
private:
	void _idPole(string basicString);
	void _nulling();

	void _setName(string & value);
	void _setCGIPath();
	void _setMetods();

	void _setCGIExtension();
};


#endif //WEBSERV_LOCATIONCONFIG_HPP
