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
	bool _bodyDelimiter;
	int _socketFD;
	int _status; // 0 wait 1 read 2 write 3 close
	int _responseCode;
	int _requestLinesParsed;
	std::string _request;
	std::string _responce;
public:
	Client(int fd): _bodyDelimiter(false), _socketFD(fd), _status(READING), _responseCode(-1), _requestLinesParsed(0){
	};
	~Client(){};

	bool getBodyDelimiterStatus(){return _bodyDelimiter;};
	int getResponseCode() const {return _responseCode;};
	const std::string & getRequest() const {return _request;};
	std::string getRequestBody(){return _requestBody;};
	std::string getResponce(){return _responce;};
	int getSocketFD() const {return _socketFD;};
	int getStatus() const {return _status;};
	void iterRequestLinesParsed(){_requestLinesParsed++;};
	int getRequestLinesParsed(){return _requestLinesParsed;};
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
		this->cleanResponce();
		_responce = resp;
	};
	void setRequestBody(const std::string & body){
		_requestBody.clear();
		_requestBody = body;
	};
	void setBodyDelimiterStatus(bool s){
		_bodyDelimiter = s;};
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
	void resetResponseData(){
		_bodyDelimiter= false;
		_status = READING;
		_responseCode= -1;
		_requestLinesParsed = 0;
		_requestType = "";
		_requestOption = "";
		_requestHTTPVersion = "";
		_requestHost = "";
		_requestBody = "";
		_request = "";
		_responce = "";
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
		if(it->getRequest().find('\n') != std::string::npos)
		{
			parceRequest(it);
			std::cout << "current parsed info:\n" <<it->getSocketFD() <<"\n"<< it->getRequestType() <<"\n"<< it->getRequestOption() <<"\n"<< it->getRequestHTTPVesion() <<"\n"<< it->getRequestHost() << std::endl;
			if(it->getBodyDelimiterStatus())
				it->setStatus(READING_DONE);
		}
	}
	void generateResponse(std::vector<Client>::iterator it)
	{
		char *bufResp = 0;
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
		it->setResponce(bufResp);
		it->setStatus(WRITING);
	}

	static void parceRequestTypeOptionVersion(std::string str,std::vector<Client>::iterator& it){
		size_t pos = str.find(' ');
		if(pos != std::string::npos) {
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

	static void parceRequest(std::vector<Client>::iterator it)
	{
		std::cout << "parse request\n";
		std::string tmp;
		std::string line;
		size_t pos;
		pos = it->getRequest().find("\r\n\r\n");
		if(!it->getBodyDelimiterStatus())
		{
			if (pos != std::string::npos) {
				std::cout << "parce req: found \\r\\n\\r\\n pos= "<< pos << "\n";
				it->setBodyDelimiterStatus(true);
				tmp = it->getRequest();
				tmp.erase(0, pos + 4);
				it->setRequestBody(tmp);
				std::cout << "req body:\n|" << it->getRequestBody() << "|\n";

				tmp = it->getRequest();
				tmp = tmp.substr(0,pos);
				tmp.append("\n");
				it->setRequest(tmp);
				std::cout << "req edited:\n|" << it->getRequest() << "|\n";
			}
		}
		if (it->getRequest().find('\n') == std::string::npos)
			std::cout << "parce req: \\n not found\n";
		while(true)
		{
			pos = it->getRequest().find('\n');
			if (pos == std::string::npos)
				return;
			if(it->getRequest() == "\r\n"){
				it->setBodyDelimiterStatus(true);
			}
			if (it->getRequest() == "\r\n")
			{
				std::cout << "parce req: \\r\\n found\n";
				return;
			}
			line = it->getRequest().substr(0,pos);
			if (it->getRequestType().empty() && !line.empty()) {
				parceRequestTypeOptionVersion(line, it);
				it->iterRequestLinesParsed();
			}
			else if(line.find("Host: ") != std::string::npos){
				line.erase(0, 6);
				it->setRequestHost(line);
				it->iterRequestLinesParsed();
			}
			tmp = it->getRequest();
			it->setRequest(tmp.erase(0, pos + 1));
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