#pragma once
#include "main.hpp"

#include "main.hpp"

class Response{
private:
	char* _response;
	ssize_t _responseSize;
	std::string _path;
	std::vector<std::string> _responseCodeMsg;
	bool _pathIsAvailable;
	bool _requestIsValid;
	bool _methodIsAllowed;
	int _outputFileFd;
	int _inputFileFd;
	bool _fileIsFound;
public:
	Response() : _response(nullptr), _responseSize(-1), _pathIsAvailable(false), _requestIsValid(true), _methodIsAllowed(false), _outputFileFd(-1), _inputFileFd(-1), _fileIsFound(false){
	};

	ssize_t getResponseSize() const
	{
		return _responseSize;
	}
	int getInputFileFd() const
	{
		return _inputFileFd;
	}
	int getOutputFileFd() const
	{
		return _outputFileFd;
	}
	void setOutputFile(int file){
		_outputFileFd = file;
	}
	void setInputFile(int file){
		_inputFileFd = file;
	}
	virtual ~Response(){};

	char* getResponse() const {return _response;};

	const std::string& getPath() const {return _path;};

	bool isPathIsAvailable() const{
		return _pathIsAvailable;
	}
	bool isFileIsFound() const{
		return _fileIsFound;
	}
	bool isMethodIsAllowed() const{
		return _methodIsAllowed;
	}
//	void setResponse(const std::string & resp){
//		this->cleanResponse();
//		_response = resp;
//	};
	void setResponse(char *resp, size_t size){
//		this->cleanResponse();
		// FIXME leaks!!!!!!!!!!!!!!!!!!!!
		_responseSize = size;
		_response = resp;
	};
	void setPath(const std::string & path){
		_path = path;
	};
//	void cleanResponse(){
//		_response.erase();
//	}

	void setPathIsAvailable(bool pathIsAvailable){
		_pathIsAvailable = pathIsAvailable;
	}

	void setMethodIsAllowed(bool methodIsAllowed){
		_methodIsAllowed = methodIsAllowed;
	}
	void setFileIsFound(bool state){
		_fileIsFound = state;
	}
	bool isRequestIsValid() const{
		return _requestIsValid;
	}

	void setRequestIsValid(bool requestIsValid){
		_requestIsValid = requestIsValid;
	}
};