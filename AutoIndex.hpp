#pragma once
#include "main.hpp"

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/* если автоиндекс включен, пользователь вводит в url директорию (http://localhost:2001/cgi-bin/)
	и в этой директории нет индекс файлов
*/
class AutoIndex {
public:
	static std::string generateAutoindexPage(std::string path) {
		// char cwd2[1024];
		// getcwd(cwd2, sizeof(cwd2));
		std::string res;
		DIR *dp;
		struct dirent *ep;
		// cwd + path
		// client.hpp 526/527
		dp = opendir(path.c_str());

		res = "<!DOCTYPE html>\n\
			<html>\n\
				<head>\n\
					<title>AutoIndex</title>\n\
				</head>\n\
				<body>\n\
					<h1>";
		res += path;
		res += "</h1>\n";

		struct stat st_buff;
		std::string type;
		if (dp != NULL)
		{
			while ((ep = readdir (dp))) {
				stat(ep->d_name, &st_buff);
					res += "<a href=\"";
					res += ep->d_name;
					res += "\">";
					res += ep->d_name;
					res += "</a> ";
					res += "<br>\n";
			}
			res += "</body>\n\
					</html>\n";

			(void) closedir (dp);
		}
		else {
			perror ("Couldn't open the directory");
			res += "Couldn't open the directory";
		}
		return res;
	}
};