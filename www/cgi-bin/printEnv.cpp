#include <iostream>

int main(int argc, char **argv, char **env) {
    std::cout << "Content-type: text/html\n";
    std::cout << "\n";
    std::cout << "<html>\n<h1>i'm cgi script from cgiTest.example</h1>\n";
    // std::cout << "<body>\n";
    // for (int i = 0; env[i] != NULL; i++) {
    //   std::cout << env[i] << "<br>\n";
    // }
    // std::cout << "</body>\n";
    std::cout << "</html>";
    return 0;
}
