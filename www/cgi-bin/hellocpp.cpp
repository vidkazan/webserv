#include <iostream>

int main(int argc, char **argv, char **env) {
  // std::string tmp;
  // std::cin >> tmp;
  std::cout << "Content-type: text/html\n";
  std::cout << "\n";
  std::cout << "<html>\n<h1>CGI env:</h1>\n";
  std::cout << "<body>\n";
  for (int i = 0; env[i] != NULL; i++) {
     std::cout << env[i] << "<br>\n";
  }
  std::cout << "</body>\n";
  std::cout << "</html>";
  return 0;
}
// #include <iostream>
// #include <vector>  
// #include <string>  
// #include <stdio.h>  
// #include <stdlib.h> 

// using namespace std;

// int main () {
   
//    cout << "Content-type:text/html\r\n\r\n";
//    cout << "<html>\n";
//    cout << "<head>\n";
//    cout << "<title>Using GET and POST Methods</title>\n";
//    cout << "</head>\n";
//    cout << "<body>\n";
//    cout << "omg i'm new html form";
//    cout << "</body>\n";
//    cout << "</html>\n";
   
//    return 0;
// }