//
// Created by Feyd Rodney on 2/1/22.
//

#ifndef PARSE_HPP
#define PARSE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "errorCodes.hpp"


using namespace std;

class formatConfigFile {
public:
	formatConfigFile(int argc, char **argv);
	virtual ~formatConfigFile();

	vector<string>  getStringServers ();
private:
	string					_fileContent;
	vector<string>			_stringServers;

	void			_formatFile ();
	void 			_bracketsCheck ();
	void			_separateStringServers ();
//	void 			_parsePoles ();
	void			_eraseExternal();
	void			_ftostr (string const & fileName);
};


#endif