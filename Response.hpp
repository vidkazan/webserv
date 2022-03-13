#pragma once
#include "main.hpp"

enum responseCodeStates{
	OK = 200,
	MOVED_PERMANENTLY = 301,
	BAD_REQUEST = 400,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	PAYLOAD_TOO_LARGE = 413,
};

class Response{
private:
	size_t _bytesSent;
	char* _response;
	ssize_t _responseSize;
 	bool _pathIsAvailable;
	bool _requestIsValid;
	bool _methodIsAllowed;
	bool _fileIsFound;
	bool _toCloseTheConnection;
	std::string _cgiOutputFileName;
	std::string _cgiInputFileName;
	responseCodeStates _responseCodes;

public:
	Response() : _bytesSent(0),\
				_response(nullptr), \
				_responseSize(-1), \
				_pathIsAvailable(false), \
				_requestIsValid(true), \
				_methodIsAllowed(false), \
				_fileIsFound(false),\
				_toCloseTheConnection(false){};
	virtual ~Response(){};

	responseCodeStates & getResponseCodes(){return _responseCodes;};
	void	setResponseCode(int code){_responseCodes = static_cast<responseCodeStates>(code);}
	bool toCloseTheConnection(){return _toCloseTheConnection;}
	void setToCloseTheConnection(bool to){_toCloseTheConnection = to;}
	void setCgiInputFileName(const std::string & name){_cgiInputFileName = name;};
	void setCgiOutputFileName(const std::string & name){_cgiOutputFileName = name;};
	std::string getCgiInputFileName(){return _cgiInputFileName;}
	std::string getCgiOutputFileName(){return _cgiOutputFileName;}
	size_t getBytesSent() const{return _bytesSent;}
	ssize_t getResponseSize() const{return _responseSize;}
	char* getResponse() const {return _response;};
	bool isPathIsAvailable() const{return _pathIsAvailable;}
	bool isFileIsFound() const{return _fileIsFound;}
	bool isMethodIsAllowed() const{return _methodIsAllowed;}
	void setResponse(char *resp, size_t size){
		_responseSize = size;
		_response = resp;
	};
	void setPathIsAvailable(bool pathIsAvailable){_pathIsAvailable = pathIsAvailable;}
	void setMethodIsAllowed(bool methodIsAllowed){_methodIsAllowed = methodIsAllowed;}
	void setFileIsFound(bool state){_fileIsFound = state;}
	bool isRequestIsValid() const{return _requestIsValid;}
	void setRequestIsValid(bool requestIsValid){_requestIsValid = requestIsValid;}
	void setBytesSent(size_t bytes){_bytesSent = bytes;}
	void addBytesSent(size_t addBytes){_bytesSent += addBytes;}
};