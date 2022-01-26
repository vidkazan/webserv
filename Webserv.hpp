#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "main.hpp"


#define RED "\e[91m"
#define YELLOW "\e[93m"
#define GREEN "\e[92m"
#define WHITE "\e[39m"

#define READING 0
#define READING_DONE 1
#define WRITING 2
#define CLOSING 3

#define REQUEST_READ_WAITING_FOR_HEADER 10
#define REQUEST_READ_HEADER 11
#define REQUEST_READ_BODY 12
#define REQUEST_READ_CHUNKED 13
#define REQUEST_READ_COMLETE 14

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
};

class	ListenSocketConfig{
private:
	std::vector<ListenSocketConfigDirectory> _directories;
	short int _port;
	char * _ip;
public:
	ListenSocketConfig(const std::vector<ListenSocketConfigDirectory>&directories, short port, char *ip)
			: _directories(directories), _port(port), _ip(ip){}

	virtual ~ListenSocketConfig(){

	}

	const std::vector<ListenSocketConfigDirectory>&getDirectories() const{
		return _directories;
	}

	short getPort() const{
		return _port;
	}

	char *getIp() const{
		return _ip;
	}

};

class Request{
private:
	std::string _type;
	std::string _option;
	std::string _httpVersion;
	std::string _host;
	std::string _body;
	bool _transferEncodingIsChunked;
	bool _isLastChunk;
	bool _bodyDelimiter;
	std::string _buffer;
	std::string _bufferChunk;
	int _readStatus;
	ssize_t _contentLength;
public:
	Request(): _type(""),\
				_option(""),\
				_httpVersion(""),\
				_host(""),\
				_body(""),\
				_transferEncodingIsChunked(false),\
				_isLastChunk(false),\
				_bodyDelimiter(false),\
				_buffer(""),\
				_bufferChunk(""),\
				_readStatus(REQUEST_READ_WAITING_FOR_HEADER), \
				_contentLength(-1){};
	virtual ~Request(){};

	bool getBodyDelimiterStatus() const {return _bodyDelimiter;};
	const std::string & getBuffer() const {return _buffer;};
	const std::string & getBufferChunk() const {return _bufferChunk;};
	std::string getBody(){return _body;};
	std::string getType() const {return  _type;};
	std::string getOption() const {return  _option;};
	std::string getHost() const {return  _host;};
	bool getTransferEncodingIsChunked() const {return  _transferEncodingIsChunked;};
	bool getLastChunkStatus() const {return  _isLastChunk;};
	std::string getHTTPVersion(){return  _httpVersion;};

	void setTransferEncoding(bool state){
		_transferEncodingIsChunked = state;
	};
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
	void setBodyDelimiterStatus(bool s){
		_bodyDelimiter = s;};
	void setLastChunkStatus(bool s){
		_isLastChunk = s;};
	void setType(const std::string & type){_type = type;};
	void setOption(const std::string & opt){_option = opt;};
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

	void setContentLength(ssize_t contentLength){
		_contentLength = contentLength;
	}
};

class Response{
private:
	std::string _response;
	int _responseCode;
	bool _pathIsAvailable;
	bool _requestIsValid;
	bool _methodIsAllowed;
public:
	Response() : _response(""), _responseCode(-1), _pathIsAvailable(false), _requestIsValid(true), _methodIsAllowed(false){};

	virtual ~Response(){};

	int getResponseCode() const {return _responseCode;};

	const std::string& getResponse() const {return _response;};

	bool isPathIsAvailable() const{
		return _pathIsAvailable;
	}

	bool isMethodIsAllowed() const{
		return _methodIsAllowed;
	}

	void setResponseCode(int code){
		if(code > 99 && code < 600)
			_responseCode = code;
	};
	void setResponse(const std::string & resp){
		this->cleanResponse();
		_response = resp;
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

class Client {
private:
	int _socketFD;
	int _status;
	ListenSocketConfig _serverConfig;
	Response _response;
	Request _request;
public:
	Client(int fd, const ListenSocketConfig& config): _socketFD(fd), _status(READING), _serverConfig(config){
	};
	~Client(){};
	int getStatus() const {return _status;};
	void resetData(){
		_status = READING;
	}

	const Response&getResponse() const{
		return _response;
	}
	const Request&getRequest() const{
		return _request;
	}
	int getSocketFd() const{
		return _socketFD;
	}

	void setStatus(int status){
		_status = status;
	};

	void setResponse(const Response&response){
		_response = response;
	}
	void readRequest(){
		char buf[100000];
		bzero(&buf, 100000);
		ssize_t ret = read(_socketFD, &buf, 100000);
//		printLog(nullptr, buf,YELLOW);
		if(ret == -1 || ret == 0){
			std::cout << "fd " << _socketFD << " status: closing\n";
			_status = CLOSING;
			return;
		}
		_request.setBuffer(_request.getBuffer() + buf);
		std::cout << "read ret: " << ret <<"\n";
		printLog("requestBuffer:", (char *)_request.getBuffer().c_str(),RED);

		if(_request.getBuffer().find("\r\n\r\n") != std::string::npos && _request.getReadStatus() == REQUEST_READ_WAITING_FOR_HEADER){
			std::cout << "Request read status: REQUEST_READ_HEADER\n";
			_request.setReadStatus(REQUEST_READ_HEADER);
		}
		if(_request.getReadStatus() == REQUEST_READ_HEADER)
		{
			parseRequestHeader();
//			std::cout << "current parsed info:" << _socketFD <<"\n" \
//						<<"type: "<< _request.getType() \
//						<<"\n"<< _request.getOption() \
//						<<"\nhttp: "<< _request.getHTTPVersion() \
//						<<"\nhost: "<< _request.getHost() \
//						<<"\nchunked: " << _request.getTransferEncodingIsChunked()\
//						<<"\ncontent-length: "<< _request.getContentLength() \
//						<< std::endl;
			}
		if(_request.getReadStatus() == REQUEST_READ_BODY)
		{
			
		}
		else if(_request.getReadStatus() == REQUEST_READ_CHUNKED)
			parseRequestBodyChunked();
		if(_request.getReadStatus() == REQUEST_READ_COMLETE) {
			write(2, "reading done\n", 13);
			_status = READING_DONE;
		}
	}

	void generateResponse()
	{
		char *bufResp = nullptr;
		if(!_response.isRequestIsValid())
		{
			bufResp = "HTTP/1.1 400 Bad Request\n"
					  "Content-Length: 64\n"
					  "Content-Type: text/html\n\n"
					  "<html>\n"
					  "<body>\n"
					  "<h1>Hello, World!!!!!!!!!!!!!</h1>\n"
					  "</body>\n"
					  "</html>";
		}
		else if(!_response.isPathIsAvailable())
		{
			bufResp = "HTTP/1.1 404 Not found\n"
					  "Content-Length: 64\n"
					  "Content-Type: text/html\n\n"
					  "<html>\n"
					  "<body>\n"
					  "<h1>Hello, World!!!!!!!!!!!!!</h1>\n"
					  "</body>\n"
					  "</html>";
		}
		else if(!_response.isMethodIsAllowed())
		{
			bufResp = "HTTP/1.1 405 Method Not Allowed\n"
							 "Content-Length: 64\n"
							 "Content-Type: text/html\n\n"
							 "<html>\n"
							 "<body>\n"
							 "<h1>Hello, World!!!!!!!!!!!!!</h1>\n"
							 "</body>\n"
							 "</html>";
		}
		else
		{
			bufResp = "HTTP/1.1 200 OK\n"
							 "Content-Length: 64\n"
							 "Content-Type: text/html\n\n"
							 "<html>\n"
							 "<body>\n"
							 "<h1>Hello, World!!!!!!!!!!!!!</h1>\n"
							 "</body>\n"
							 "</html>";
		}
		std::string resp = bufResp;
		_response.setResponse(bufResp);
		_status = WRITING;
		Request request;
		_request = request;
	}

	void parseRequestTypeOptionVersion(std::string str)
	{
		size_t pos = str.find(' ');
		if(pos != std::string::npos) {
			_request.setType(str.substr(0, pos));
			str.erase(0,pos+1);
		}
		pos = str.find(' ');
		if(pos != std::string::npos){
			_request.setOption(str.substr(0, pos));
			str.erase(0,pos+1);
		}
		if(!str.empty())
			_request.setHTTPVersion(str);
	}

	void analyseRequest(){
		std::cout << "analyse request:\n";
		if(_request.getType().empty() || _request.getOption().empty() || _request.getHTTPVersion().empty() \
				|| _request.getHost().empty() ){
			_response.setRequestIsValid(false);
		}
		std::vector<ListenSocketConfigDirectory>::const_iterator it = _serverConfig.getDirectories().begin();
		std::vector<ListenSocketConfigDirectory>::const_iterator itEnd = _serverConfig.getDirectories().end();
		for(;it != itEnd; it++){
			if(it->getDirectoryName() == _request.getOption())
			{
				_response.setPathIsAvailable(true);
				if(it->getDirectoryAllowedMethods().find(_request.getType()) != std::string::npos)
					_response.setMethodIsAllowed(true);
			}
		}
	}

	void parseRequestBody(){}

	void parseRequestBodyChunked()
	{
		std::string tmp;
		size_t pos;
		static long chunkSize = -1;
		while(_request.getBufferChunk().empty())
		{
			tmp = _request.getBuffer();
			if((pos = tmp.find("\r\n")) == std::string::npos)
				return;
			std::istringstream(tmp.substr(0, pos)) >> std::hex >> chunkSize;
			if(chunkSize == 0)
			{
				if(_request.getBuffer().find("0\r\n\r\n") != std::string::npos && _request.getBuffer().size() == 5)
				{
					write(2, "parse req: last chunk found\n",28);
					_request.setLastChunkStatus(true);
					std::cout << "Request read status: REQUEST_READ_COMPLETE\n";
					_request.setReadStatus(REQUEST_READ_COMLETE);
					return;
				}
				std::cout << pos << " chunkSize 0\n";
				return;
			}
			std::cout << pos << " chunkSize: " << chunkSize << " size: " <<  tmp.size() << "\n";
			if((chunkSize + pos + 2) <= tmp.size())
			{
				_request.setBufferChunk(tmp.substr(pos + 1, chunkSize));
				std::cout << "chunkBuffer:\n|" << _request.getBufferChunk() << "|\n" << "chunkBufferSize:\n" << _request.getBufferChunk().size() << "\n" ;
				_request.setBuffer(tmp.erase(0, chunkSize + pos + 4));
				exportChunk();
				_request.setBufferChunk("");
				printLog("requestBuffer:", (char *)_request.getBuffer().c_str(),RED);
			}
			else
				return;
		}
	}
	void exportChunk(){}
	void parseRequestHeader()
	{
		std::string tmp;
		std::string line;
		size_t pos;
		while(true)
		{
			if(_request.getBuffer().find("\r\n") == 0)
			{
				tmp = _request.getBuffer();
				_request.setBuffer(tmp.erase(0, 2));
				if (_request.getReadStatus() != REQUEST_READ_BODY && _request.getReadStatus() != REQUEST_READ_CHUNKED)
					_request.setReadStatus(REQUEST_READ_COMLETE);
//				printLog("requestBuffer:", (char *)_request.getBuffer().c_str(),RED);
				return;
			}
			pos = _request.getBuffer().find('\n');
			line = _request.getBuffer().substr(0,pos);
			if (_request.getType().empty() && !line.empty()) {
				parseRequestTypeOptionVersion(line);
			}
			else if(line.find("Host: ") != std::string::npos){
				line.erase(0, 6);
				_request.setHost(line);
			}
			else if(line.find("Transfer-Encoding: ") != std::string::npos){
				line.erase(0, 19);
				if (line.find("chunked") != std::string::npos)
				{
					_request.setTransferEncoding(true);
					std::cout << "Request read status: REQUEST_READ_CHUNKED\n";
					_request.setReadStatus(REQUEST_READ_CHUNKED);
				}
			}
			else if(line.find("Content-Length: ") != std::string::npos){
				line.erase(0, 15);
				_request.setContentLength(std::stoi(line));
				std::cout << "Request read status: REQUEST_READ_BODY\n";
				_request.setReadStatus(REQUEST_READ_BODY);
			}
			tmp = _request.getBuffer();
			_request.setBuffer(tmp.erase(0, pos + 1));
//			printLog("requestBuffer:", (char *)_request.getBuffer().c_str(),RED);
		}
	}
};


class ListenSocket {
private:
	int _socketFD;
	struct sockaddr_in _adr;
	ListenSocketConfig _config;
public:
	ListenSocket(const ListenSocketConfig & config): _config(config){
		_adr.sin_addr.s_addr = 0;
		_adr.sin_family = 0;
		_adr.sin_len = 0;
		_adr.sin_port = 0;

		_socketFD = socket(AF_INET, SOCK_STREAM, 0);
		int opt = 1;
		setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
		_adr.sin_family = AF_INET;
		_adr.sin_port = htons(_config.getPort());
		_adr.sin_addr.s_addr = inet_addr(_config.getIp());
	};
	~ListenSocket(){};
	int getSocketFD() const {return _socketFD;};
	struct sockaddr_in& getSockAddrInStruct(){return _adr;};

	const ListenSocketConfig&getConfig() const{
		return _config;
	}
};

class Webserv {
private:
	std::vector<ListenSocket> _servSockets;
	std::vector<Client> _clientSockets;
public:
	Webserv(){};
	~Webserv(){};
	void configFileParse(){};
	std::vector<ListenSocket>& getServSockets(){return _servSockets;};
	std::vector<Client>& getClientSockets(){return _clientSockets;};
	void errorMsg(const char *msg){write(2, msg, strlen(msg));};
	void addListenSocket(const ListenSocketConfig & config){
		ListenSocket socket(config);
		_servSockets.push_back(socket);
		if(bind(_servSockets.back().getSocketFD(), (struct sockaddr *)&_servSockets.back().getSockAddrInStruct(), sizeof(_servSockets.back().getSockAddrInStruct())) < 0){
			perror("bind error");
			exit(EXIT_FAILURE);
		}
	}
	void addClientSocket(int fd,const ListenSocketConfig& config){
		Client clientSocket(fd, config);
		_clientSockets.push_back(clientSocket);
	}
};

void printLog(char *description,char *msg, char *color){
	write(2, color, strlen(color));
	if(description && *description)
	{
		write(2, description, strlen(description));
		write(2, "\n", 1);
	}
	write(2, "|", 1);
	write(2, msg, strlen(msg));
	write(2, "|", 1);
	write(2, WHITE, 5);
	write(2, "\n", 1);
}

#endif