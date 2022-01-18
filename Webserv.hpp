#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "main.hpp"
#include <cstdio>

#define RED "\e[91m"
#define YELLOW "\e[93m"
#define GREEN "\e[92m"
#define WHITE "\e[39m"
#define READING 1
#define WRITING 2
#define CLOSING 3

class Client {
private:
	std::string _requestType;
	std::string _requestOption;
	std::string _requestHTTPVersion;
	std::string _requestHost;
	int _socketFD;
	int _status; // 0 wait 1 read 2 write 3 close
	int _responseCode;
	std::string _request;
	std::string _responce;
public:
	Client(int fd): _socketFD(fd), _status(READING), _responseCode(-1){
	};
	~Client(){};

	int getResponseCode() const {return _responseCode;};
	std::string getRequest(){return _request;};
	std::string getResponce(){return _responce;};
	int getSocketFD() const {return _socketFD;};
	int getStatus() const {return _status;}
	std::string getRequestType() const {return  _requestType;};
	std::string getRequestOption() const {return  _requestOption;};
	std::string getRequestHost() const {return  _requestHost;};
	std::string getRequestHTTPVesion(){return  _requestHTTPVersion;};

	void setResponseCose(int code){
		if(code > 99 && code < 600)
			_responseCode = code;
	};
	void setStatus(int status){
		_status = status;
	};
	void setRequest(const std::string & req){
		this->cleanRequest();
		_request = req;
	};
	void setResponce(const std::string & resp){
		_responce = resp;
	};
	void setRequestType(const std::string & type){_requestType = type;};
	void setRequestOption(const std::string & opt){_requestOption = opt;};
	void setRequestHTTPVesion(const std::string & version){_requestHTTPVersion = version;};
	void setRequestHost(const std::string & host){_requestHost = host;};
	void cleanRequest(){
		_request.erase();
	}
	void cleanResponce(){
		_responce.erase();
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
	void configFileParce(){};
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
		it->setRequest(it->getRequest() + buf);
		std::cout << "read ret: " << ret <<"\n";
		printLog("request:", (char *)it->getRequest().c_str(),RED);
		if(it->getRequest().find('\n') >= 0)
		{
			parceRequest(it);
			std::cout <<it->getSocketFD() <<"\n"<< it->getRequestType() <<"\n"<< it->getRequestOption() <<"\n"<< it->getRequestHTTPVesion() <<"\n"<< it->getRequestHost() <<"\n";
			it->setStatus(WRITING);
		}
	}
	void generateResponse(std::vector<Client>::iterator it){
		char bufResp[] = "HTTP/1.1 200 OK\n"
						 "Content-Length: 64\n"
						 "Content-Type: text/html\n\n"
						 "<html>\n"
						 "<body>\n"
						 "<h1>Hello, World!!!!!!!!!!!!!</h1>\n"
						 "</body>\n"
						 "</html>";
		std::string resp = bufResp;
		it->setResponce(resp);
	}

	static void parceRequestTypeOptionVersion(std::string str,std::vector<Client>::iterator& it){
		size_t pos = str.find(' ');
		if(pos >= 0) {
			it->setRequestType(str.substr(0, pos));
			str.erase(0,pos+1);
		}
		else
		{
			it->setResponseCose(400);
			return;
		}
		pos = str.find(' ');
		if(pos != std::string::npos){
			it->setRequestOption(str.substr(0, pos));
			str.erase(0,pos+1);
		}
		else
		{
			it->setResponseCose(400);
			return;
		}
		if(!str.empty())
			it->setRequestHTTPVesion(str);
		else
		{
			it->setResponseCose(400);
			return;
		}
	}

	static void parceRequest(std::vector<Client>::iterator it){
		std::string line;
		size_t pos;

		while(true)
		{
			pos = it->getRequest().find('\n');
			if (pos == std::string::npos)
				return;
			line = it->getRequest().substr(0,pos);
			if (it->getRequestType().empty() && !line.empty()) {
				parceRequestTypeOptionVersion(line, it);
			}
			else if(line.find("Host: ") != std::string::npos){
				line.erase(0, 6);
				it->setRequestHost(line);
			}
			it->setRequest(it->getRequest().erase(0, pos + 1));
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
	write(2, msg, strlen(msg));
	write(2, WHITE, 5);
	write(2, "\n", 1);
}

#endif