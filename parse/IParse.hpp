//
// Created by Feyd Rodney on 3/1/22.
//

#ifndef WEBSERV_IPARSE_HPP
#define WEBSERV_IPARSE_HPP

#pragma once
#include "errorCodes.hpp"
enum tumbler {OFF, ON};
using namespace std;
#include <iostream>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

/*
 * Abstract class for parse scopes of server part and server:location part
 */

class IParse {
public:

	IParse() {};
	virtual ~IParse() {};

	tumbler						autoindex;
	string 						root;
	map<int, string>			error_pages;
	string						index;
	ssize_t						client_body_buffer_size;

protected:

	string			_raw;

	/*
	 * main parsing loop of raw configurations string
	 */

	void _parse();

	/*
	 * Utility methods 'get single value' and 'raw erase'(don't ask)
	 */

	string _getSingleValue();

	void _rawErase(string str);

	/*
	 * Set-methods for common fields declared in the 'protect'
	 *  TODO: set autoindex set
	 */

	void _checkDir(string dir);

    void _checkFile(string dir);

	void _setRoot();

	void _setIndex();

	void _setAutoindex();

	void _setCBBS();

//	void _setErrPage();

	virtual void _idPole(string basicString) = 0;
	virtual void _nulling() = 0;


};


#endif //WEBSERV_IPARSE_HPP
