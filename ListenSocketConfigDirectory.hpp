#pragma once

#include "main.hpp"

class ListenSocket;

class	ListenSocketConfigDirectory{
private:
	std::string _directoryName;
	std::string _directoryAllowedMethods;
	std::string _directoryPath;
public:
	ListenSocketConfigDirectory(const std::string&directoryName, const std::string&directoryAllowedMethods,
								const std::string&directoryPath) : _directoryName(directoryName),
																   _directoryAllowedMethods(directoryAllowedMethods),
																   _directoryPath(directoryPath){}

	virtual ~ListenSocketConfigDirectory(){}

	const std::string&getDirectoryName() const{
		return _directoryName;
	}

	const std::string&getDirectoryAllowedMethods() const{
		return _directoryAllowedMethods;
	}

	const std::string&getDirectoryPath() const{
		return _directoryPath;
	}

	unsigned short countSlash(const std::string & str) const
	{

		std::string subStr = str;
		std::cout << "str: " << subStr;
		unsigned short count = 0;
		size_t pos = 0;
		while(pos != std::string::npos)
		{
			subStr = subStr.substr(pos + 1, subStr.size() - pos - 1);
			pos = subStr.find('/');
			if (pos != std::string::npos && pos != (subStr.size() - 1))
			{
				count++;
			}
		}
		 std::cout << "count: " << count << "\n";
		return count;
	}

	bool operator<(ListenSocketConfigDirectory rhs) const
	{
		write(1, "here1\n", 6);
		std::string subStr;
		unsigned short countThis;
		unsigned short countRhs;
		subStr = _directoryName;
		countThis = countSlash(subStr);
		subStr = rhs.getDirectoryName();
		countRhs = countSlash(subStr);
		if(countThis < countRhs)
			return true;
		else
			return false;
	}
	bool operator>(ListenSocketConfigDirectory rhs) const
	{
		write(1, "here1\n", 6);
		std::string subStr;
		unsigned short countThis = 0;
		unsigned short countRhs = 0;
		subStr = _directoryName;
		countThis = countSlash(subStr);
		subStr = rhs.getDirectoryName();
		countRhs = countSlash(subStr);
		if(countThis > countRhs)
			return true;
		else
			return false;
	}
};