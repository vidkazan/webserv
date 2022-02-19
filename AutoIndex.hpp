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
	static std::string generateAutoindexPage(char cwd[1024], std::string path) {
		std::string res;
		// res = "<h1>";
		DIR *dp;
		struct dirent *ep;
		// cwd + path
		// client.hpp 526/527
		dp = opendir ("/Users/cvenkman/Desktop/webserv_linux/www/cgi-bin/");

		res = "<!DOCTYPE html>\n\
			<html>\n\
				<head>\n\
					<title>AutoIndex</title>\n\
				</head>\n\
				<body>\n\
					<h1>";
		res += cwd;
		res += path;
		res += "</h1>\n\
					<table class="" order="">\n\
					<tbody>\n";

		struct stat st_buff;
		std::string type;
		if (dp != NULL)
		{
			while ((ep = readdir (dp))) {
				stat(ep->d_name, &st_buff);

			switch (st_buff.st_mode & S_IFMT) {
				case S_IFBLK:  type = "block device";            break;
				case S_IFCHR:  type = "character device";        break;
				case S_IFDIR:  type = "directory";               break;
				case S_IFIFO:  type = "FIFO/pipe";               break;
				case S_IFLNK:  type = "symlink";                 break;
				case S_IFREG:  type = "regular file";            break;
				case S_IFSOCK: type = "socket";                  break;
				default:       type = "unknown?";                break;
				}
				if (type == "directory") {
					// bool isDir = true;
					// res += "<tr>";
					// res += "<td sortable-data="1www"><table class="ellipsis">";
					// res += "<tbody><tr><td><a class=\"dir\" href=\"www/\">www";
					// res += "href=\"www/\"";
					// res += "</a>";
					// res += "<td></td>";
					res += "<a class=\"dir\" href=\"";
					res += ep->d_name;
					res += "/\">";
					res += ep->d_name;
					res += "</a> ";
					res += type + " " + std::to_string(st_buff.st_size);
					// st_buff.st_size;
					res += "<br>\n";
					// res += "<a href=\"www/\"></a>\n";
				}
				else {
					// res += "";
					res += ep->d_name;
					res += " " + type + " ";
					res += std::to_string(st_buff.st_size);
					res += "<br>\n";
				}
			}
			res += \
					"</tbody>\n\
					</table>\n\
					</body>\n\
					</html>\n";

			(void) closedir (dp);
		}
		else {
			perror ("Couldn't open the directory");
			res += "Couldn't open the directory";
		}
		// res += "</h1>";
		return res;
	}
};