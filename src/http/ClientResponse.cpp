//
// Created by Dmitrii Grigorev on 27.03.22.
//
#include "../main.hpp"

void        Client::generateResponse()
{
//    std::cout << YELLOW << "id: " + std::to_string(_request.getRequestId()) + " fd: " + std::to_string(getSocketFd()) << " \n" << _request.getOption() << WHITE << "\n";
//    std::cout << YELLOW << "id: " + std::to_string(_request.getRequestId()) + " fd: " + std::to_string(getSocketFd()) << WHITE << " ";
//    printStates("");
    std::fstream inputFile;
    std::string bufResp;
    std::string body;
    switch (_request.getRequestErrors()) {
        case ERROR_REDIRECT: {
            bufResp = "HTTP/1.1 301 Moved Permanently\n";
            _response.setResponseCode(301);
            break;
        }
        case ERROR_REQUEST_NOT_VALID: {
            bufResp += "HTTP/1.1 400 Bad Request\n";
            _response.setResponseCode(400);
            break;
        }
        case ERROR_METHOD_NOT_ALLOWED: {
            bufResp = "HTTP/1.1 405 Method Not Allowed\n";
            _response.setResponseCode(405);
            break;
        }
        case ERROR_PATH_NOT_AVAILABLE:
        case ERROR_FILE_NOT_FOUND: {
            bufResp = "HTTP/1.1 404 Not found\n";
            _response.setResponseCode(404);
            break;
        }
        case ERROR_BODY_OVER_MAX_SIZE: {
            bufResp = "HTTP/1.1 413 Payload Too Large\n";
            _response.setResponseCode(413);
            break;
        }
        case NO_ERROR: {
            bufResp = "HTTP/1.1 200 OK\r\n";
            _response.setResponseCode(200);
            break;
        }
    }
    switch (_request.getRequestMethod()){
        case HEAD:
        case GET:
        {
            switch(_response.getResponseCodes()){
                case 0:
                    break;
                case 301: {
                    bufResp += "Location: ";
                    bufResp += _request.getRedirect();
                    bufResp += "\r\n";
                    inputFile.open(_serverConfig.getErrorPages(301), std::ios::in);
                    break;
                }
                case 400: {
                    inputFile.open(_serverConfig.getErrorPages(400), std::ios::in);
                    break;
                }
                case 404: {
                    inputFile.open(_serverConfig.getErrorPages(404), std::ios::in);
                    break;
                }
                case 405: {
                    inputFile.open(_serverConfig.getErrorPages(405), std::ios::in);
                    break;
                }
                case 200: {
                    switch (_request.getRequestOptionType()) {
                        case OPTION_DIR: {
                            if (!_request.getDirectoryConfig().getDirectoryIndexName().empty())
                                inputFile.open(
                                        _request.getFullPath() + _request.getDirectoryConfig().getDirectoryIndexName(),
                                        std::ios::in);
                            else if (_request.isAutoIndex())//если индекс файла нет, записываем автоиндекс в body
                                body = AutoIndex::generateAutoindexPage(_request.getFullPath());
                            break;
                        }
                        case OPTION_FILE: {
                            inputFile.open(_request.getFullPath(), std::ios::in);
                            break;
                        }
                        case NO_OPTION:
                            break;
                        case OPTION_CGI: {
                            CGI *cgi = new CGI(_request.getType(), _request.getFullPath(), \
                                                    _response.getCgiOutputFileName(), _response.getCgiInputFileName(), \
                                                    _serverConfig);
                            try {
                                cgi->executeCgiScript();
//                                bufResp = cgi->getBufResp();
                                body = cgi->getBody();
                            }
                            catch (const std::exception &e) {
                                std::cerr << "ERROR IN CGI: " << e.what() << '\n';
                                bufResp = "HTTP/1.1 500 Internal Server Error\n";
                                bufResp += CGI::error500ContentType;
                                bufResp += "\n";
                                body = CGI::error500Body;
                            }
                            delete cgi;
                        }
                    }
                    break;
                }
                case 413:
                {
                    inputFile.open(_serverConfig.getErrorPages(413), std::ios::in);
                    break;
                }
            }
            break;
        }
        case PUT:
        case POST:
        {
            switch(_response.getResponseCodes()) {
                case 0:{
                    break;
                }
                case 301: {
                    bufResp += "Location: ";
                    bufResp += _request.getRedirect();
                    bufResp += "\r\n";
                    inputFile.open(_serverConfig.getErrorPages(301), std::ios::in);
                    break;
                }
                case 400: {
                    inputFile.open(_serverConfig.getErrorPages(400), std::ios::in);
                    break;
                }
                case 404: {
                    inputFile.open(_serverConfig.getErrorPages(404), std::ios::in);
                    break;
                }
                case 405: {
                    inputFile.open(_serverConfig.getErrorPages(405), std::ios::in);
                    break;
                }
                case 200: {
                    switch (_request.getRequestOptionType()) {
                        case OPTION_CGI: {
                            CGI *cgi = new CGI(_request.getType(), _request.getFullPath(), \
                                                        _response.getCgiOutputFileName(),
                                               _response.getCgiInputFileName(), _serverConfig);
                            try {
                                cgi->executeCgiScript();
                                //                        bufResp += cgi->getContentTypeStr();
                                //                        bufResp += "\n";
                                body = cgi->getBody();
                                std::remove(_response.getCgiInputFileName().c_str());
                                std::remove(_response.getCgiOutputFileName().c_str());
                            }
                            catch (const std::exception &e) {
                                std::cerr << "ERROR IN CGI: " << e.what() << '\n';
                                bufResp = "HTTP/1.1 500 Internal Server Error\n";
                                bufResp += CGI::error500ContentType;
                                bufResp += "\n";
                                body = CGI::error500Body;
                            }
                            delete cgi;
                            break;
                        }
                        case OPTION_DIR: {
                            if(!_request.getMultiPartFileName().empty())
                            {
                                inputFile.open(SUCCESS_UPLOAD_FILE_PATH_NAME, std::ios::in);
                                std::stringstream buffer;
                                buffer << inputFile.rdbuf();
                                body = buffer.str();
                                body.replace(body.find("/fnm/"), 5, _request.getMultiPartFileName());
                            }
                            break;

                        }
                        case OPTION_FILE:
                        case NO_OPTION:
                            break;
                    }
                    break;
                }
                case 413: {
                    inputFile.open(_serverConfig.getErrorPages(413), std::ios::in);
                    break;
                }
            }
            break;
        }
        case NO_METHOD:
        case DELETE:
        case OTHER_METHOD:
        {
            if(_response.getResponseCodes() == 400)
                inputFile.open(_serverConfig.getErrorPages(400), std::ios::in);
            break;
        }
    }
    if(_request.getRequestMethod() != HEAD)
    {
        std::string cgiContentType;
        if (_request.getRequestOptionType() == OPTION_CGI) {
                if (body.length() > 3) {
                std::string tmpContentType(body, 0, body.find_first_of("\n\n") + 1);
                cgiContentType = tmpContentType;
                std::string tmpBody(body, body.find_first_of("\n\n") + 1, body.length());
                body = tmpBody;
            }
        }

        std::stringstream buffer;
        buffer << inputFile.rdbuf();
        if (body.empty())
            body = buffer.str();
        bufResp += "Content-Length: ";
        bufResp += std::to_string((unsigned long long) body.size());
        bufResp += "\n";
        if (_request.getOptionFileExtension() == "html")
            bufResp += "Content-Type: text/html\n";
        else if (_request.getOptionFileExtension() == "png")
            bufResp += "Content-Type: image/png\n";
        else if (_request.getRequestOptionType() == OPTION_CGI)
            bufResp += cgiContentType;
        bufResp += "\n";
        bufResp += body;
    }
    else
    {
        bufResp += "Content-Length: 0\n\n";
    }
    allocateResponse(bufResp);
//    std::ofstream logfile;
//    logfile.open(RESPONSE_LOG_FILE_PATH_NAME + std::to_string(_request.getRequestId()) + LOG_FILE_EXTENSION, std::ios::trunc);
//    logfile << bufResp;
//    logfile.close();
    _status = WRITING;
    inputFile.close();
    
//    std::string a1 = RESPONSE_LOG_FILE_PATH_NAME + std::to_string(_request.getRequestId()) + LOG_FILE_EXTENSION;
//    std::string a2 = REQUEST_LOG_FILE_PATH_NAME + std::to_string(_request.getRequestId()) + LOG_FILE_EXTENSION;
//            std::remove(a2.c_str());
    if(_request.getRequestMethod() != POST)
    {
//            std::remove(a1.c_str());
    }
}

void        Client::allocateResponse(std::string bufResp){
    char *res;
    size_t i=0;
    size_t size = bufResp.size();
    res = (char *)malloc(sizeof (char) * (size));
    for(;i < size;i++){
        res[i] = bufResp[i];
    }
    _response.setResponse(res,i);
}
void        Client::sendResponse()
{
    ssize_t ret;
    ssize_t size = 0;
    if(_response.getResponseSize() - _response.getBytesSent() < 1000000000)
        size = _response.getResponseSize() - _response.getBytesSent();
    else
        size = 999999999;
    ret = send(_socketFD, _response.getResponse() + _response.getBytesSent(),size,0x80000); //  SIGPIPE ignore
    if(ret <= 0)
    {
        Response response;
        setResponse(response);
        Request request;
        _request = request;
        free(_response.getResponse());
        setStatus(CLOSING);
        return;
    }

    _response.addBytesSent(ret);
    if(_response.getBytesSent() == (size_t)_response.getResponseSize())
    {
//        std::cout << GREEN << "id: " + std::to_string(_request.getRequestId()) + " fd: " + std::to_string(getSocketFd()) << " " << "sent: " << _response.getBytesSent() << WHITE << "\n";
        if(_response.toCloseTheConnection())
            setStatus(CLOSING);
        else
            setStatus(READING);
        free(_response.getResponse());
        Response response;
        setResponse(response);
        Request request;
        _request = request;
    }
}