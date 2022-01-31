#pragma once
#include "main.hpp"

#include "main.hpp"

class Response{
private:
	std::string _response;
	std::string _path;
	std::vector<std::string> _responseCodeMsg;
	bool _pathIsAvailable;
	bool _requestIsValid;
	bool _methodIsAllowed;
	int _outputFileFd;
	int _inputFileFd;
public:
	Response() : _response(""), _pathIsAvailable(false), _requestIsValid(true), _methodIsAllowed(false), _outputFileFd(-1), _inputFileFd(-1){
	};


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

	const std::string& getResponse() const {return _response;};

	const std::string& getPath() const {return _path;};

	bool isPathIsAvailable() const{
		return _pathIsAvailable;
	}

	bool isMethodIsAllowed() const{
		return _methodIsAllowed;
	}
	void setResponse(const std::string & resp){
		this->cleanResponse();
		_response = resp;
	};
	void setPath(const std::string & path){
		_path = path;
	};
	void cleanResponse(){
		_response.erase();
	}

	void setPathIsAvailable(bool pathIsAvailable){
		_pathIsAvailable = pathIsAvailable;
	}

	void setMethodIsAllowed(bool methodIsAllowed){
		_methodIsAllowed = methodIsAllowed;
	}

	bool isRequestIsValid() const{
		return _requestIsValid;
	}

	void setRequestIsValid(bool requestIsValid){
		_requestIsValid = requestIsValid;
	}
};