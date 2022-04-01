#include <iostream>

int main(int argc, char **argv, char **env) {
    std::cout << "Content-type: text/html\n";
    std::cout << "\r\n";
    std::cout << "<html>\n<h1>i'm cgi script from printEnv.bla and this is my env:</h1>\n";
     std::cout << "<body>\n";
     for (int i = 0; env[i] != NULL; i++) {
       std::cout << env[i] << "<br>\n";
     }
     std::cout << "</body>\n";
    std::cout << "</html>";
    return 0;
}
