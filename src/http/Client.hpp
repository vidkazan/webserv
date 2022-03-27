#pragma once
#include "../main.hpp"

class AutoIndex;
class Client {
private:
	int                              _socketFD;
	int                              _status;
	std::vector<VirtualServerConfig> _virtualServers;
	VirtualServerConfig              _serverConfig;
	Response                         _response;
	Request                          _request;

public:
	            Client(int fd, std::vector<VirtualServerConfig> virtualServers): _socketFD(fd), _status(READING), _virtualServers(virtualServers) {};
	            ~Client(){};
	int         getStatus() const {return _status;};
	int         getSocketFd() const{return _socketFD;}
	void        setStatus(int status){_status = status;}
	void        setVirtualServerConfig(const VirtualServerConfig & conf){_serverConfig = conf;}
    void        setResponse(const Response&response){_response = response;}
    void        printStates(std::string place)
    {
        std::cout << YELLOW << place << "\n";
        switch (_request.getReadStatus()) {
            case 0:
                std::cout << "REQUEST_READ_WAITING_FOR_HEADER ";
                break;
            case 1:
                std::cout << "REQUEST_READ_HEADER ";
                break;
            case 2:
                std::cout << "REQUEST_READ_BODY ";
                break;
            case 3:
                std::cout << "REQUEST_READ_CHUNKED ";
                break;
            case 4:
                std::cout << "REQUEST_READ_MULTIPART ";
                break;
            case 5:
                std::cout << "REQUEST_READ_COMPLETE ";
                break;
        }
        std::cout << "\n";
        switch (_request.getRequestMethod()) {
            case 0:
                std::cout << "NO_METHOD ";
                break;
            case 1:
                std::cout << "GET ";
                break;
            case 2:
                std::cout << "POST ";
                break;
            case 3:
                std::cout << "PUT ";
                break;
            case 4:
                std::cout << "DELETE ";
                break;
            case 5:std::cout << "HEAD ";
                break;
            case 6:
                std::cout << "OTHER_METHOD ";
                break;
        }
        std::cout << "\n";
        switch (_request.getRequestBodyType()) {
            case 0:
                std::cout << "NO_BODY ";
                break;
            case 1:
                std::cout << "OPTION_DIR ";
                break;
            case 2:
                std::cout << "OPTION_CGI ";
                break;
            case 3:
                std::cout << "OPTION_FILE ";
                break;
        }
        std::cout << "\n";
        switch (_request.getRequestOptionType()) {
            case 0:
                std::cout << "NO_OPTION ";
                break;
            case 1:
                std::cout << "OPTION_DIR ";
                break;
            case 2:
                std::cout << "OPTION_CGI ";
                break;
            case 3:
                std::cout << "OPTION_FILE ";
                break;
        }
        std::cout << "\n";
        switch (_request.getRequestErrors()) {
            case 0:
                std::cout << "NO_ERROR ";
                break;
            case 1:
                std::cout << "ERROR_REQUEST_NOT_VALID ";
                break;
            case 2:
                std::cout << "ERROR_METHOD_NOT_ALLOWED ";
                break;
            case 3:
                std::cout << "ERROR_PATH_NOT_AVAILABLE ";
                break;
            case 4:
                std::cout << "ERROR_BODY_OVER_MAX_SIZE ";
                break;
            case 5:
                std::cout << "ERROR_FILE_NOT_FOUND ";
                break;
            case 6:
                std::cout << "ERROR_REDIRECT ";
                break;
        }
        std::cout << WHITE << "\n";
    }

//              ClientRequestParse.cpp

    void        parseRequestHeader(std::ofstream * file);
    void        parseRequestTypeOptionVersion(std::string str);
    void        parseRequestBody();
    void        parseRequestMultiPart(std::ofstream * file);
    void        parseRequestBodyChunked(std::ofstream * file);
    void        exportChunk();

//              ClientRequestAnalyse.cpp

    void        analyseRequest(std::ofstream * file);
    void        analysePath(std::ofstream * file);
    void        findVirtualServer();
    bool        isCgi();

//              ClientResponse.cpp

    void        generateResponse();
    void        allocateResponse(std::string bufResp);
    void        sendResponse();

//              ClientReadRequest.cpp

    void        readRequest();
    void        recvBuffer();
};

