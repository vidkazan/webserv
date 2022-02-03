#pragma once
#include "main.hpp"

class Request{
private:
	// header
	std::string _type;
	std::string _option;
	std::string _httpVersion;
	std::string _host;
	std::string _body;
	std::string _buffer;
	std::string _bufferChunk;
	int _readStatus;
	ssize_t _contentLength;
	ssize_t _chunkSize;
	bool _isCgi;
	bool _isDirectory;
	bool _isXSecretHeader;
	size_t _count;
	std::string _optionPath;
	std::string _optionFileName;
	std::string _optionFileExtension;
	std::string _fullPath;
public:
	Request(): _type(""),\
				_option(""),\
				_httpVersion(""),\
				_host(""),\
				_body(""),\
				_buffer(""),\
				_bufferChunk(""),\
				_readStatus(REQUEST_READ_WAITING_FOR_HEADER), \
				_contentLength(-1), \
				_chunkSize(-1), \
				_isCgi(0), \
				_isDirectory(0), \
				_isXSecretHeader(0), \
				_count(0){};
	virtual ~Request()
	{
	}
	const std::string & getBuffer() const {return _buffer;};
	const std::string & getBufferChunk() const {return _bufferChunk;};
	std::string getBody(){return _body;};
	std::string getType() const {return  _type;};
	std::string getOption() const {return  _option;};

	std::string getOptionPath() const {return  _optionPath;};
	std::string getOptionFileName() const {return  _optionFileName;};
	std::string getOptionFileExtension() const {return  _optionFileExtension;};

	std::string getHost() const {return  _host;};
	std::string getHTTPVersion(){return  _httpVersion;};
	bool isXSecretHeader(){return _isXSecretHeader;}
	bool isCgi(){return _isCgi;}

	bool isDirectory(){return _isDirectory;}
	void setIsXSecretHeader(bool x){_isXSecretHeader = x;}

	void setIsCgi(bool cgi){_isCgi = cgi;}
	void setIsDirectory(bool dir){_isDirectory = dir;}
	void setBuffer(const std::string & req){
		this->cleanBuffer();
		_buffer = req;
	};
	void setBufferChunk(const std::string & buf){
		_bufferChunk.erase();
		_bufferChunk = buf;
	};
	void setBody(const std::string & body){
		_body.clear();
		_body = body;
	};
	void setType(const std::string & type){_type = type;};
	void setOption(const std::string & opt){_option = opt;};

	void setOptionPath(const std::string & opt){_optionPath = opt;};
	void setOptionFileName(const std::string & opt){_optionFileName = opt;};
	void setOptionFileExtension(const std::string & opt){_optionFileExtension = opt;};

	void setHTTPVersion(const std::string & version){_httpVersion = version;};
	void setHost(const std::string & host){_host = host;};
	void cleanBuffer(){
		_buffer.erase();
	}
	int getReadStatus() const{
		return _readStatus;
	}
	void setReadStatus(int readStatus){
		_readStatus = readStatus;
	}

	ssize_t getContentLength() const{
		return _contentLength;
	}

	size_t getCounter() const{
		return _count;
	}

	void setContentLength(ssize_t contentLength){
		_contentLength = contentLength;
	}

	void setCounter(size_t count){
		_count = count;
	}

	ssize_t getChunkSize() const{
		return _chunkSize;
	}

	void setChunkSize(ssize_t chunkSize){
		_chunkSize = chunkSize;
	}
	void setFullPath(const std::string & path){
		_fullPath = path;
	};

	const std::string& getFullPath() const {return _fullPath;};
};