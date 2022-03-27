//
// Created by Dmitrii Grigorev on 27.03.22.
//
#include "../main.hpp"

void        Client::generateResponse()
{
    std::cout << YELLOW << "id: " + std::to_string(_request.getRequestId()) + " fd: " + std::to_string(getSocketFd()) << " \n" << _request.getOption() << WHITE << "\n";
    std::cout << YELLOW << "id: " + std::to_string(_request.getRequestId()) + " fd: " + std::to_string(getSocketFd()) << WHITE << " ";
    printStates("");
    std::fstream inputFile;
    std::string bufResp;
    std::string body;

    switch (_request.getRequestErrors()) {
        case ERROR_REDIRECT:
            bufResp = "HTTP/1.1 301 Moved Permanently\n";
            _response.setResponseCode(301);
            break;
        case ERROR_REQUEST_NOT_VALID:
            bufResp += "HTTP/1.1 400 Bad Request\n";
            _response.setResponseCode(400);
            break;
        case ERROR_METHOD_NOT_ALLOWED:
            bufResp = "HTTP/1.1 405 Method Not Allowed\n";
            _response.setResponseCode(405);
            break;
        case ERROR_PATH_NOT_AVAILABLE:
        case ERROR_FILE_NOT_FOUND:
            bufResp = "HTTP/1.1 404 Not found\n";
            _response.setResponseCode(404);
            break;
        case ERROR_BODY_OVER_MAX_SIZE:
            bufResp = "HTTP/1.1 413 Payload Too Large\n";
            _response.setResponseCode(413);
            break;
        case NO_ERROR:
            bufResp = "HTTP/1.1 200 OK\r\n";
            _response.setResponseCode(200);
            break;
    }

    switch (_request.getRequestMethod()){
        case HEAD:
        case GET:
        {
            switch(_response.getResponseCodes()){
                case 301:
                    bufResp += "Location: ";
                    bufResp += _request.getRedirect();
                    bufResp += "\r\n";
                    inputFile.open("www/301.html", std::ios::in);
                    break;
                case 400:
                    inputFile.open("www/400.html", std::ios::in);
                    break;
                case 404:
                    inputFile.open("www/404.html", std::ios::in);
                    break;
                case 405:
                    inputFile.open("www/405.html", std::ios::in);
                    break;
                case 200:
                    switch (_request.getRequestOptionType()){
                        case OPTION_DIR: {
                            if (!_request.getDirectoryConfig().getDirectoryIndexName().empty())
                                inputFile.open(_request.getFullPath() + _request.getDirectoryConfig().getDirectoryIndexName(), std::ios::in);
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
                        case OPTION_CGI:
                        {
								std::cout << "cgi--- " << _request.getFullPath() << "\n";
                            CGI *cgi = new CGI(_request.getType(), _request.getFullPath(), \
                                                    _response.getCgiOutputFileName(), _response.getCgiInputFileName());
                            try {
                                cgi->executeCgiScript();
                                 bufResp += cgi->getContentTypeStr();
                                 bufResp += "\n";
                                bufResp = cgi->getBufResp();
                                body = cgi->getBody();
                            }
                            catch (const std::exception &e) {
                                /*
                                    в классе CGI есть статические перемнные
                                    с готовым body и сторокой с ContentType при ошибке 500
                                */
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
                case 413:
                    break;
            }
//            if(_request.getRequestOptionType() != OPTION_CGI) {
                std::stringstream buffer;
                buffer << inputFile.rdbuf();
                if (body.empty())
                    body = buffer.str();
//            }
            break;
        }
        case PUT:
        case POST:
        {
            switch (_request.getRequestOptionType()){
                case OPTION_CGI:
                {
                    std::cout << "cgi--- " << _request.getFullPath() << "\n";
                    CGI *cgi = new CGI(_request.getType(), _request.getFullPath(), \
                                                    _response.getCgiOutputFileName(), _response.getCgiInputFileName());
                    try {
                        cgi->executeCgiScript();
                                bufResp += cgi->getContentTypeStr();
                                bufResp += "\n";
                        body = cgi->getBody();
                        std::remove(_response.getCgiInputFileName().c_str());
                        std::remove(_response.getCgiOutputFileName().c_str());
                    }
                    catch (const std::exception &e) {
                        /*
                            в классе CGI есть статические перемнные
                            с готовым body и сторокой с ContentType при ошибке 500
                        */
                        std::cerr << "ERROR IN CGI: " << e.what() << '\n';
                        bufResp = "HTTP/1.1 500 Internal Server Error\n";
                        bufResp += CGI::error500ContentType;
                        bufResp += "\n";
                        body = CGI::error500Body;
                    }
                    delete cgi;
                }
                    break;
                case OPTION_DIR:{ // multipart
                    inputFile.open("www/uploadSuccess.html", std::ios::in);
                    std::stringstream buffer;
                    buffer << inputFile.rdbuf();
                    body = buffer.str();
                    body.replace(body.find("/fnm/"),5,_request.getMultiPartFileName());
                    break;
                }
                case OPTION_FILE:
                case NO_OPTION:
                    break;
            }
        }
        case NO_METHOD:
        case DELETE:
        case OTHER_METHOD:
            break;
    }
    bufResp += "Content-Length: ";
    bufResp += std::to_string((unsigned  long long )body.size());
    bufResp += "\n";
    bufResp += "Content-Type: ";
    //choosing type
    if (_request.getOptionFileExtension() == "html")
        bufResp += "text/html";
    if (_request.getOptionFileExtension() == "png")
        bufResp += "image/png";
    bufResp += "\n\n";
    if(_request.getRequestMethod() != HEAD)
        bufResp += body;
    allocateResponse(bufResp);
    std::ofstream logfile;
    logfile.open("tmp/log/resp_" + std::to_string(_request.getRequestId()) + ".txt", std::ios::trunc);
    logfile << bufResp;
    logfile.close();
    _status = WRITING;
    inputFile.close();
    std::string a1 = "tmp/log/resp_" + std::to_string(_request.getRequestId()) + ".txt";
    std::string a2 = "tmp/log/fullReq_" + std::to_string(_request.getRequestId()) + ".txt";
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
    ssize_t ret = send(_socketFD, _response.getResponse() + _response.getBytesSent(),_response.getResponseSize() - _response.getBytesSent(),MSG_NOSIGNAL); //  SIGPIPE ignore
    if(ret <= 0)
    {
        free(_response.getResponse());
        setStatus(CLOSING);
        return;
    }
    _response.addBytesSent(ret);
    if(_response.getBytesSent() == (size_t)_response.getResponseSize())
    {
        std::cout << GREEN << "id: " + std::to_string(_request.getRequestId()) + " fd: " + std::to_string(getSocketFd()) << " " << "sent: " << _response.getBytesSent() << WHITE << "\n";
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