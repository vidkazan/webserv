#pragma once
#include "main.hpp"

enum RequestMethod{
	NO_METHOD,
	GET,
	POST,
	PUT,
	DELETE,
	HEAD,
	OTHER_METHOD
};

enum ReadStatus{
	REQUEST_READ_WAITING_FOR_HEADER,
 	REQUEST_READ_HEADER,
 	REQUEST_READ_BODY,
 	REQUEST_READ_CHUNKED,
 	REQUEST_READ_MULTIPART,
 	REQUEST_READ_COMPLETE,
};

enum RequestErrors {
	NO_ERROR,
	ERROR_REQUEST_NOT_VALID,
	ERROR_METHOD_NOT_ALLOWED,
	ERROR_PATH_NOT_AVAILABLE,
	ERROR_BODY_OVER_MAX_SIZE,
	ERROR_FILE_NOT_FOUND,
	ERROR_REDIRECT
};
enum RequestBodyType {
	NO_BODY,
	BODY_BASE,
	BODY_CHUNKED,
	BODY_MULTI_PART
};

enum RequestOptionType {
	NO_OPTION,
	OPTION_DIR,
	OPTION_CGI,
	OPTION_FILE,
};

class Request{
private:
	// header
	short _id;
	std::string _type;
	std::string _option;
	std::string _httpVersion;
	std::string _host;
	std::string _body;
	std::string _buffer;
	std::string _bufferChunk;
	std::string _multiPartFileName;

	ssize_t _contentLength;
	ssize_t _chunkSize;
	bool _isAutoIndex;
	bool _isXSecretHeader;

	std::string _redirect;
	ssize_t _maxBodySize;
	size_t _count;
	std::string _optionPath;
	std::string _optionFileName;
	std::string _optionFileExtension;
	std::string _fullPath;
	ReadStatus		_readStatus;
	RequestMethod	_requestMethod;
	RequestErrors	_requestErrors;
	RequestBodyType _requestBodyType;
	RequestOptionType	_requestOptionType;
    VirtualServerConfigDirectory _dirConfig;
public:
	Request():	_id(0), \
				_type(""),\
				_option(""),\
				_httpVersion(""),\
				_host(""),\
				_body(""),\
				_buffer(""),\
				_bufferChunk(""),\
				_contentLength(-1), \
				_chunkSize(-1), \
				_redirect(""), \
				_isAutoIndex(0), \
				_isXSecretHeader(0), \
				_maxBodySize(-1), \

				_count(0), \
				_readStatus(REQUEST_READ_WAITING_FOR_HEADER), \
				_requestMethod(NO_METHOD), \
				_requestErrors(NO_ERROR), \
				_requestBodyType(NO_BODY), \
				_requestOptionType(NO_OPTION)
	{
		while(_id < 1){
			_id = rand();
		}
	};

	ReadStatus getReadStatus() const{return _readStatus;}
	RequestMethod & getRequestMethod(){return _requestMethod;};
	RequestErrors & getRequestErrors(){return _requestErrors;};
	RequestBodyType & getRequestBodyType(){return _requestBodyType;};
	RequestOptionType & getRequestOptionType(){return _requestOptionType;};
	void	setReadStatus(int readStatus){_readStatus = static_cast<ReadStatus>(readStatus);}
	void	setRequestMethod(int m){_requestMethod = static_cast<RequestMethod>(m);}
	void	setRequestErrors(int m){_requestErrors = static_cast<RequestErrors>(m);}
	void	setRequestBodyType(int m){_requestBodyType= static_cast<RequestBodyType>(m);}
	void	setRequestOptionType(int m){_requestOptionType = static_cast<RequestOptionType>(m);}

	virtual ~Request(){}
	short getRequestId(){return _id;};
	const std::string & getBuffer() const {return _buffer;};
	const std::string & getBufferChunk() const {return _bufferChunk;};
	std::string getBody(){return _body;};
	std::string getMultiPartFileName(){return _multiPartFileName;};
	std::string getType() const {return  _type;};
	std::string getOption() const {return  _option;};
	std::string getOptionPath() const {return  _optionPath;};
	std::string getOptionFileName() const {return  _optionFileName;};
	std::string getOptionFileExtension() const {return  _optionFileExtension;};
	std::string getHost() const {return  _host;};
	std::string getHTTPVersion(){return  _httpVersion;};
	bool isXSecretHeader(){return _isXSecretHeader;}

	bool isAutoIndex(){return _isAutoIndex;}
	std::string getRedirect(){return _redirect;}
	void setIsXSecretHeader(bool x){_isXSecretHeader = x;}
	void setRedirect(std::string redir){_redirect = redir;}
	void setIsAutoIndex(bool ai){_isAutoIndex = ai;}

	void setBuffer(const std::string & req){
		this->cleanBuffer();
		_buffer = req;
	};
	void appendBuffer(char *str, size_t size){
		_buffer.append(str, size);
	}
	void setBufferChunk(const std::string & buf){
		_bufferChunk = buf;
	};
	void setMultiPartFileName(const std::string & name){
		_multiPartFileName = name;
	};
	void setType(const std::string & type){_type = type;};
	void setOption(const std::string & opt){_option = opt;};
	void setOptionPath(const std::string & opt){_optionPath = opt;};
	void setOptionFileName(const std::string & opt){_optionFileName = opt;};
	void setOptionFileExtension(const std::string & opt){_optionFileExtension = opt;};
	void setHTTPVersion(const std::string & version){_httpVersion = version;};
	void setHost(const std::string & host){_host = host;};
	void cleanBuffer(){_buffer.erase();}
	ssize_t getContentLength() const{return _contentLength;}
	size_t getCounter() const{return _count;}
	ssize_t getMaxBodySize() const{return _maxBodySize;}
	void setMaxBodySize(ssize_t size){_maxBodySize = size;}
	void setContentLength(ssize_t contentLength){_contentLength = contentLength;}
	void setCounter(size_t count){_count = count;}
	ssize_t getChunkSize() const{return _chunkSize;}
	void setChunkSize(ssize_t chunkSize){_chunkSize = chunkSize;}
	void setFullPath(const std::string & path){_fullPath = path;};
	const std::string& getFullPath() const {return _fullPath;};

    VirtualServerConfigDirectory getDirectoryConfig(){return _dirConfig;};
    void setDirectoryConfig(const VirtualServerConfigDirectory&conf){_dirConfig = conf;};
};