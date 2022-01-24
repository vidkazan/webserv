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
public:
	Request(): _type(""),\
				_option(""),\
				_httpVersion(""),\
				_host(""),\
				_body(""),\
				_transferEncodingIsChunked(false),\
				_isLastChunk(false),\
				_bodyDelimiter(false),\
				_buffer(""){};
	virtual ~Request(){};

	bool getBodyDelimiterStatus() const {return _bodyDelimiter;};
	const std::string & getBuffer() const {return _buffer;};
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
};

class Response{
private:
	std::string _response;
	int _responseCode;
public:
	Response() : _response(""), _responseCode(-1){};

	virtual ~Response(){};

	int getResponseCode() const {return _responseCode;};
	const std::string& getResponse() const {return _response;};

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
};

class Client {
private:
	int _socketFD;
	int _status;
	Response _response;
	Request _request;
public:
	Client(int fd): _socketFD(fd), _status(READING){
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
		printLog(nullptr, buf,YELLOW);
		if(ret == -1 || ret == 0){
			std::cout << "fd " << _socketFD << " status: closing\n";
			_status = CLOSING;
			return;
		}
		_request.setBuffer(_request.getBuffer() + buf);
		std::cout << "read ret: " << ret <<"\n";
		printLog("requestBuffer:", (char *)_request.getBuffer().c_str(),RED);
		if(_request.getBuffer().find('\n') != std::string::npos)
		{
			parseRequest();
			std::cout << "current parsed info:" << _socketFD <<"\n"<< _request.getType() <<"\n"<< _request.getOption() <<"\n"<< _request.getHTTPVersion() <<"\n"<< _request.getHost()<<"\nchunked: " << _request.getTransferEncodingIsChunked() << std::endl;
			if((_request.getBodyDelimiterStatus() && !_request.getTransferEncodingIsChunked()) || (_request.getTransferEncodingIsChunked() && _request.getLastChunkStatus())) {
				write(2, "reading done\n", 13);
				_status = READING_DONE;
			}
		}
	}

	void generateResponse()
	{
		char *bufResp = nullptr;
		if(_request.getOption() != "/")

		if(_request.getType().empty())
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
		if(_request.getType() == "POST" && _request.getOption() == "/")
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
		else if(_request.getType() == "GET")
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
		else if(_request.getType() == "HEAD"  && _request.getOption() == "/")
		{
			bufResp = "HTTP/1.1 405 Method Not Allowed\n"
					  "Server: webserv/1.0\n"
					  "Date: Mon, 17 Jan 2022 16:29:40 GMT\n"
					  "Accept-Charsets: utf-8\n"
					  "Accept-Language: en-US\n"
					  "Allow: GET \n"
					  "Content-Length: 0\n\n";
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
		else
		{
			_response.setResponseCode(400);
			return;
		}
		pos = str.find(' ');
		if(pos != std::string::npos){
			_request.setOption(str.substr(0, pos));
			str.erase(0,pos+1);
		}
		else
		{
			_response.setResponseCode(400);
			return;
		}
		if(!str.empty())
			_request.setHTTPVersion(str);
		else
		{
			_response.setResponseCode(400);
			return;
		}
	}

	void	checkPath(){

	}

	void	analyseRequest(){
		checkPath();
	}

	void parseRequest()
	{
		std::cout << "Req parsing...\n";
		std::string tmp;
		std::string line;
		size_t pos;
		if(!_request.getBodyDelimiterStatus())
		{
			pos = _request.getBuffer().find("\r\n\r\n");
			if (pos != std::string::npos)
			{
				std::cout << "parse req: found \\r\\n\\r\\n pos " << pos << "\n";
				_request.setBodyDelimiterStatus(true);
				tmp = _request.getBuffer();
				tmp.erase(0, pos + 4);
				_request.setBody(tmp);
				std::cout << "req body:\n|" << _request.getBody() << "|\n";
				tmp = _request.getBuffer();
				tmp = tmp.substr(0,pos);
				tmp.append("\n");
				_request.setBuffer(tmp);
				std::cout << "req edited:\n|" << _request.getBuffer() << "|\n";
			}
		}
		if(_request.getTransferEncodingIsChunked()){
			if((_request.getBody().find("0\r\n\r\n") != std::string::npos) || (_request.getBuffer().find("0\r\n\r\n") != std::string::npos))
			{
				write(2, "parse req: last chunk found\n",28);
				_request.setLastChunkStatus(true);
			}
		}
		while(true)
		{
			pos = _request.getBuffer().find('\n');
			if (pos == std::string::npos) {
				return;
			}
			if(_request.getBuffer() == "\r\n" && !_request.getBodyDelimiterStatus()){
				_request.setBodyDelimiterStatus(true);
				std::cout << "parse req: \\r\\n found\n";
			}
			line = _request.getBuffer().substr(0,pos);
			if (_request.getType().empty() && !line.empty()) {
				parseRequestTypeOptionVersion(line);
			}
			else if(line.find("Host: ") != std::string::npos){
				line.erase(0, 6);
				_request.setHost(line);
			}
			else if(line.find("Transfer-Encoding: ") != std::string::npos){
				line.erase(0,19);
				if(line.find("chunked") != std::string::npos)
					_request.setTransferEncoding(true);
			}
			if(!(_request.getBodyDelimiterStatus() && _request.getTransferEncodingIsChunked())) {
				tmp = _request.getBuffer();
				_request.setBuffer(tmp.erase(0, pos + 1));
			}
			else
				return;
		}
	}
};

class ListenSocket {
private:
	int _socketFD;
	struct sockaddr_in _adr;
public:
	ListenSocket(int portNum, const char *stringIP){
		_adr.sin_addr.s_addr = 0;
		_adr.sin_family = 0;
		_adr.sin_len = 0;
		_adr.sin_port = 0;

		_socketFD = socket(AF_INET, SOCK_STREAM, 0);
		int opt = 1;
		setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
		_adr.sin_family = AF_INET;
		_adr.sin_port = htons(portNum);
		_adr.sin_addr.s_addr = inet_addr(stringIP);
	};
	~ListenSocket(){};
	int getSocketFD() const {return _socketFD;};
	struct sockaddr_in& getSockAddrInStruct(){return _adr;};
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
	void addListenSocket(int portNum, char *stringIP){
		ListenSocket socket(portNum, stringIP);
		_servSockets.push_back(socket);
		if(bind(_servSockets.back().getSocketFD(), (struct sockaddr *)&_servSockets.back().getSockAddrInStruct(), sizeof(_servSockets.back().getSockAddrInStruct())) < 0){
			perror("bind error");
			exit(EXIT_FAILURE);
		}
	}
	void addClientSocket(int fd){
		Client clientSocket(fd);
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