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
#include <map>
#include <vector>

/*
 * Abstract class for parse scopes of server part and server:location part
 */

class IParse {
public:

	IParse() {};
	virtual ~IParse() {};

	tumbler				autoindex;
	string 				root;
	map<int, string>	error_pages;
	string				index;
	ssize_t				client_body_buffer_size;

protected:

	string				_raw;

	/*
	 * main parsing loop of raw configurations string
	 */

	void _parse() {
		string	pole;
		size_t 	pos;

		while (!this->_raw.empty()){
			if ((pos = this->_raw.find(" ")) && pos != string::npos)
				pole = this->_raw.substr(0, pos);
			else
				throw std::runtime_error (ERR_INTERPRET);
			this->_idPole(pole);
		}
	}

	/*
	 * Utility methods 'get single value' and 'raw erase'(don't ask)
	 */

	string _getSingleValue() {
		size_t	pos;
		bool 	notLast = false;
		string	value;

		if ((pos = this->_raw.find(" ")) != string::npos) {
			value = this->_raw.substr(0, pos);
			notLast = true;
		}
		else if (!this->_raw.empty())
			value = this->_raw;
		else
			throw std::runtime_error (ERR_PARSE);
		if (notLast)
			this->_rawErase(value+" ");
		else
			this->_rawErase(value);
		return value;
	}

	void _rawErase(string str) {
		this->_raw.erase(0, str.length());
	}

	/*
	 * Set-methods for common fields declared in the 'protect'
	 *  TODO: set autoindex set
	 */

	void _checkDir(string dir) {
		struct stat s;
		if (stat(dir.c_str(),&s) || !S_ISDIR(s.st_mode))
			throw std::runtime_error (string (ERR_INVALID_DIR) + ": '" + dir + "'" );
	}

	void _checkFile(string dir) {
		struct stat s;
		if (stat(dir.c_str(),&s) || !S_ISREG(s.st_mode))
			throw std::runtime_error (string (ERR_INVALID_FILE) + ": '" + dir + "'" );
	}

	void _setRoot() {
		// this->_rawErase("root ");
		// string root = this->_getSingleValue();

		// _checkDir(root);
		// this->root = root;
		this->_rawErase("root ");
		string root = this->_getSingleValue();

		_checkDir(root);
		if (*(root.end() - 1) != '/')
			root += "/";
		this->root = root;

	}

	void _setIndex() {
		if (!this->index.empty())
			throw std::runtime_error (string (ERR_DOUBLE_ASSIGMENT) + ": '");

		this->_rawErase("index ");

		string value = this->_getSingleValue();

		/*
		 * check index file in main location
		 */

		ifstream file;
		file.open((this->root + value).c_str()); // TODO допилить права онли на чтение
		file.close();
		if(!file)
			throw std::runtime_error(string(ERR_INVALID) + ": index file '" + value+ "'");
		this->index = value;
	}

	void _setAutoindex() {
		this->_rawErase("autoindex ");
		string state  = this->_getSingleValue();
		if (state == "on")
			this->autoindex = ON;
		else if (state == "off") {
			this->autoindex = OFF;
		}
		else
			throw std::runtime_error (string(ERR_INTERPRET) + ": '" + state + "'");
	}

	void _setCBBS() {
		if (this->client_body_buffer_size != -1)
			throw std::runtime_error(ERR_DOUBLE_ASSIGMENT);

		this->_rawErase("client_body_buffer_size ");

		string value = this->_getSingleValue();

		size_t pos;

		this->client_body_buffer_size = \
		static_cast<ssize_t>(stoi(value, &pos, 10));
		if (this->client_body_buffer_size < 0 || pos != value.length())
			throw std::runtime_error(ERR_INVALID);
	}

	void _setErrPage() {
		this->_rawErase("error_page ");
		vector<int> codes;

		string value = this->_getSingleValue();

		size_t number_of_characters;
		int code = stoi(value, &number_of_characters);
		if (number_of_characters != value.length())
			throw std::runtime_error(string(ERR_INVALID) + ": error_page code '" + value+ "'");
		codes.push_back(code);

		while (42) {
			value = this->_getSingleValue();
			try {
				code = stoi(value, &number_of_characters);
			}
			catch (exception & ex) {
				break;
			}
			if (number_of_characters != value.length())
				break;
			codes.push_back(code);
		}
		ifstream file;
		file.open((this->root + value).c_str()); // TODO допилить права онли на чтение
		file.close();
		if(!file)
			throw std::runtime_error(string(ERR_INVALID) + ": error_page file '" + value+ "'");
		for (vector<int>::iterator it = codes.begin(); it != codes.end(); it++)
			error_pages[*it] = value;

	}

	virtual void _idPole(string basicString) = 0;
	virtual void _nulling() = 0;


};


#endif //WEBSERV_IPARSE_HPP
