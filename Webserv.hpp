#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "main.hpp"
#include <cstdio>

#define RED "\e[91m"
#define YELLOW "\e[93m"
#define GREEN "\e[92m"
#define WHITE "\e[39m"
#define READING 0
#define READING_DONE 1
#define WRITING 2
#define CLOSING 3

class Client {
private:
	std::string _requestType;
	std::string _requestOption;
	std::string _requestHTTPVersion;
	std::string _requestHost;
	std::string _requestBody;
	bool _requestTransferEncodingIsChunked;
	bool _isLastChunk;
	bool _bodyDelimiter;
	int _socketFD;
	int _status;
	int _responseCode;
	std::string _requestBuffer;
	std::string _response;
public:
	Client(int fd): _requestTransferEncodingIsChunked(false), _isLastChunk(false),_bodyDelimiter(false), _socketFD(fd), _status(READING), _responseCode(-1){
	};
	~Client(){};

	bool getBodyDelimiterStatus() const {return _bodyDelimiter;};
	int getResponseCode() const {return _responseCode;};
	const std::string & getRequestBuffer() const {return _requestBuffer;};
	std::string getRequestBody(){return _requestBody;};
	std::string getResponse(){return _response;};
	int getSocketFD() const {return _socketFD;};
	int getStatus() const {return _status;};
	std::string getRequestType() const {return  _requestType;};
	std::string getRequestOption() const {return  _requestOption;};
	std::string getRequestHost() const {return  _requestHost;};
	bool getRequestTransferEncodingIsChunked() const {return  _requestTransferEncodingIsChunked;};
	bool getLastChunkStatus() const {return  _isLastChunk;};
	std::string getRequestHTTPVersion(){return  _requestHTTPVersion;};


	void setResponseCode(int code){
		if(code > 99 && code < 600)
			_responseCode = code;
	};
	void setStatus(int status){
		_status = status;
	};
	void setRequestTransferEncoding(bool state){
		_requestTransferEncodingIsChunked= state;
	};
	void setRequestBuffer(const std::string & req){
		this->cleanRequestBuffer();
		_requestBuffer= req;
	};
	void setResponse(const std::string & resp){
		this->cleanResponse();
		_response = resp;
	};
	void setRequestBody(const std::string & body){
		_requestBody.clear();
		_requestBody = body;
	};
	void setBodyDelimiterStatus(bool s){
		_bodyDelimiter = s;};
	void setLastChunkStatus(bool s){
		_isLastChunk = s;};
	void setRequestType(const std::string & type){_requestType = type;};
	void setRequestOption(const std::string & opt){_requestOption = opt;};
	void setRequestHTTPVersion(const std::string & version){_requestHTTPVersion = version;};
	void setRequestHost(const std::string & host){_requestHost = host;};
	void cleanRequestBuffer(){
		_requestBuffer.erase();
	}
	void cleanResponse(){
		_response.erase();
	}
	void resetRequestData(){
		_isLastChunk = false;
		_bodyDelimiter= false;
		_status = READING;
		_responseCode= -1;
		_requestType = "";
		_requestOption = "";
		_requestHTTPVersion = "";
		_requestHost = "";
		_requestBody = "";
		_requestBuffer= "";
		_requestTransferEncodingIsChunked = false;
		_response = "";
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
	void readRequest(std::vector<Client>::iterator it){
		char buf[100000];
		bzero(&buf, 100000);
		ssize_t ret = read(it->getSocketFD(), &buf, 100000);
		printLog(nullptr, buf,YELLOW);
		if(ret == -1 || ret == 0){
			std::cout << "fd " << it->getSocketFD() << " status: closing\n";
			it->setStatus(CLOSING);
			return;
		}
		it->setRequestBuffer(it->getRequestBuffer() + buf);
		std::cout << "read ret: " << ret <<"\n";
		printLog("requestBuffer:", (char *)it->getRequestBuffer().c_str(),RED);
		if(it->getRequestBuffer().find('\n') != std::string::npos)
		{
			parseRequest(it);
//			std::cout << "current parsed info:" <<it->getSocketFD() <<"\n"<< it->getRequestType() <<"\n"<< it->getRequestOption() <<"\n"<< it->getRequestHTTPVersion() <<"\n"<< it->getRequestHost()<<"\nchunked: " << it->getRequestTransferEncodingIsChunked() << std::endl;
			if((it->getBodyDelimiterStatus() && !it->getRequestTransferEncodingIsChunked()) || (it->getRequestTransferEncodingIsChunked() && it->getLastChunkStatus())) {
				write(2, "reading done\n", 13);
				it->setStatus(READING_DONE);
			}
		}
	}
	void generateResponse(std::vector<Client>::iterator it)
	{
		char *bufResp = nullptr;
		if(it->getRequestType().empty())
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
		if(it->getRequestType() == "POST" && it->getRequestOption() == "/")
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
		else if(it->getRequestType() == "GET")
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
		else if(it->getRequestType() == "HEAD"  && it->getRequestOption() == "/")
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
		it->setResponse(bufResp);
		it->setStatus(WRITING);
	}

	static void parseRequestTypeOptionVersion(std::string str,std::vector<Client>::iterator& it){
		size_t pos = str.find(' ');
		if(pos != std::string::npos) {
			it->setRequestType(str.substr(0, pos));
			str.erase(0,pos+1);
		}
		else
		{
			it->setResponseCode(400);
			return;
		}
		pos = str.find(' ');
		if(pos != std::string::npos){
			it->setRequestOption(str.substr(0, pos));
			str.erase(0,pos+1);
		}
		else
		{
			it->setResponseCode(400);
			return;
		}
		if(!str.empty())
			it->setRequestHTTPVersion(str);
		else
		{
			it->setResponseCode(400);
			return;
		}
	}

	static void parseRequest(std::vector<Client>::iterator it)
	{
		std::cout << "Req parsing...\n";
		std::string tmp;
		std::string line;
		size_t pos;
		if(!it->getBodyDelimiterStatus())
		{
			pos = it->getRequestBuffer().find("\r\n\r\n");
			if (pos != std::string::npos)
			{
				std::cout << "parse req: found \\r\\n\\r\\n pos " << pos << "\n";
				it->setBodyDelimiterStatus(true);
				tmp = it->getRequestBuffer();
				tmp.erase(0, pos + 4);
				it->setRequestBody(tmp);
				std::cout << "req body:\n|" << it->getRequestBody() << "|\n";
				tmp = it->getRequestBuffer();
				tmp = tmp.substr(0,pos);
				tmp.append("\n");
				it->setRequestBuffer(tmp);
//				std::cout << "req edited:\n|" << it->getRequestBuffer() << "|\n";
			}
		}
		if(it->getRequestTransferEncodingIsChunked()){
			if((it->getRequestBody().find("0\r\n\r\n") != std::string::npos) || (it->getRequestBuffer().find("0\r\n\r\n") != std::string::npos))
			{
				write(2, "parse req: last chunk found\n",28);
				it->setLastChunkStatus(true);
			}
		}
//		if (it->getRequestBuffer().find('\n') == std::string::npos)
//			std::cout << "parse req: \\n not found\n";
		while(true)
		{
			pos = it->getRequestBuffer().find('\n');
			if (pos == std::string::npos) {
				return;
			}
			if(it->getRequestBuffer() == "\r\n" && !it->getBodyDelimiterStatus()){
				it->setBodyDelimiterStatus(true);
				std::cout << "parse req: \\r\\n found\n";
			}
			line = it->getRequestBuffer().substr(0,pos);
			if (it->getRequestType().empty() && !line.empty()) {
				parseRequestTypeOptionVersion(line, it);
			}
			else if(line.find("Host: ") != std::string::npos){
				line.erase(0, 6);
				it->setRequestHost(line);
			}
			else if(line.find("Transfer-Encoding: ") != std::string::npos){
				line.erase(0,19);
				if(line.find("chunked") != std::string::npos)
					it->setRequestTransferEncoding(true);
			}
			if(!(it->getBodyDelimiterStatus() && it->getRequestTransferEncodingIsChunked())) {
				tmp = it->getRequestBuffer();
				it->setRequestBuffer(tmp.erase(0, pos + 1));
			}
			else
				return;
		}
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