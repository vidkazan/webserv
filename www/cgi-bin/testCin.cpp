#include <iostream>

int main(int argc, char **argv, char **env) {
	std::string tmp;
	std::cin >> tmp;
	std::cout << "Content-type: text/html\n";
	std::cout << "\n";
	std::cout << "<html>\n<h1>CGI env:</h1>\n";
	std::cout << "<body>\n";
	std::cout << tmp << "\n";
	std::cout << "</body>\n";
	std::cout << "</html>";
	return 0;
}