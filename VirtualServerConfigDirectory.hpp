#pragma once
#include "main.hpp"

class VirtualServerConfigDirectory
{
	private:
		std::string _directoryName;
		std::string _directoryAllowedMethods;
		std::string _directoryPath;
		std::string _directoryRedirect;
		ssize_t		_maxBodySize;
	public:
		VirtualServerConfigDirectory(const std::string&directoryName, const std::string&directoryAllowedMethods,
							  const std::string&directoryPath,const std::string&directoryRedirect, ssize_t bodySize) : \
							  _directoryName(directoryName), \
							  _directoryAllowedMethods(directoryAllowedMethods), \
							  _directoryPath(directoryPath), \
							  _directoryRedirect(directoryRedirect), \
							  _maxBodySize(bodySize){}

		~VirtualServerConfigDirectory(){}

		const std::string&getDirectoryName() const{return _directoryName;}
		const std::string&getDirectoryAllowedMethods() const{return _directoryAllowedMethods;}
		const std::string&getDirectoryPath() const{return _directoryPath;}
		const std::string&getDirectoryRedirect() const{return _directoryRedirect;}
		ssize_t getMaxBodySize() const{return _maxBodySize;}
		unsigned short countSlash(const std::string & str) const
		{
			std::string subStr = str;
			unsigned short count = 0;
			size_t pos = 0;
			while(pos != std::string::npos)
			{
				subStr = subStr.substr(pos + 1, subStr.size() - pos - 1);
				pos = subStr.find('/');
				if (pos != std::string::npos && pos != (subStr.size() - 1)){
					count++;
				}
			}
			return count;
		}
		bool operator<(VirtualServerConfigDirectory rhs) const
		{
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
		bool operator>(VirtualServerConfigDirectory rhs) const
		{
			std::string subStr;
			unsigned short countThis;
			unsigned short countRhs;
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