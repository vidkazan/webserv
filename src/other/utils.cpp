#include "../main.hpp"

void printLog(std::string description,std::string msg,std::string color){
	std::cout << color;
	if(!description.empty())
		std::cout << description << "\n";
	std::cout << "|" << msg << "|" << WHITE << "\n";
}