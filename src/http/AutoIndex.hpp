#pragma once
#include "../main.hpp"

class AutoIndex {
public:
	static std::string generateAutoindexPage(std::string path) {
		// char cwd2[1024];
		// getcwd(cwd2, sizeof(cwd2));
		bool isDoubleSlash = false;
		if (path[path.size() - 1] == '/' && path[path.size() - 2] == '/') {
			std::string tmpPath(path, 0, path.size() - 1);
			path = tmpPath;
			isDoubleSlash = true;
		}
		std::string dirName(path, path.find_first_of("/") + 1, path.length());

		std::string res;
		DIR *dir;
		struct dirent *current;
		dir = opendir(path.c_str());

		res = "<!DOCTYPE html>\n\
			<html>\n\
				<head>\n\
					<title>AutoIndex</title>\n\
				</head>\n\
				<body>\n\
					<h1>";
		res += path;
		res += "</h1>\n";

		if (dir != NULL)
		{
			current = readdir(dir);
			while ((current = readdir(dir))) {
					res += "<a href=\"";
					if (!strcmp(current->d_name, "..") || isDoubleSlash == true)
						res += current->d_name;
					else
						res += dirName + current->d_name;
					res += "\">";
					res += current->d_name;
					res += "</a> ";
					res += "<br>\n";
			}
			closedir(dir);
		}
		else {
			perror ("Couldn't open the directory");
			res += "Couldn't open the directory";
		}
		res += "</body>\n\
				</html>\n";
		return res;
	}
};
