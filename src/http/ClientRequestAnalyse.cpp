//
// Created by Dmitrii Grigorev on 27.03.22.
//
#include "../main.hpp"

void        Client::analyseRequest()
{
    if(_request.getRequestMethod() == NO_METHOD || _request.getOption().empty() || _request.getHTTPVersion().empty() || _request.getHost().empty() )
    {
        _response.setRequestIsValid(false);
        _request.setRequestErrors(ERROR_REQUEST_NOT_VALID);
        return;
    }
    findVirtualServer();
    analysePath();

    if(!_response.isMethodIsAllowed())
        _request.setRequestErrors(ERROR_METHOD_NOT_ALLOWED);

    if(!_response.isPathIsAvailable())
        _request.setRequestErrors(ERROR_PATH_NOT_AVAILABLE);

    if (this->isCgi())
        _request.setRequestOptionType(OPTION_CGI);

    if(_request.getRequestErrors() != NO_ERROR)
        return;

    switch (_request.getRequestOptionType()) {
        case OPTION_CGI:
        case OPTION_DIR:
            break;
        default:
            _request.setRequestOptionType(OPTION_FILE);
    }
    switch (_request.getRequestOptionType()) {
        case NO_OPTION:
        case OPTION_DIR:
            break;
        case OPTION_CGI: {
            _request.setFullPath(_request.getDirectoryConfig().getCgiPath());
            std::stringstream OutputFileName;
            std::ofstream cgiOutput;
            OutputFileName << "tmp/tmp_cgi_output_";
            OutputFileName << _socketFD;
            _response.setCgiOutputFileName(OutputFileName.str());
            cgiOutput.open(_response.getCgiOutputFileName(), std::ios::trunc);
            if (!cgiOutput.is_open())
                std::cout << _response.getCgiOutputFileName() << " analyse request: cgi: tmp output file open error\n";

            std::stringstream InputFileName;
            std::ofstream cgiInput;
            InputFileName << "tmp/tmp_cgi_input_";
            InputFileName << _socketFD;
            _response.setCgiInputFileName(InputFileName.str());
            cgiInput.open(_response.getCgiInputFileName(), std::ios::trunc);
            if (!cgiInput.is_open())
                std::cout << _response.getCgiInputFileName() << " analyse request: cgi: tmp input file open error\n";
            break;
        }
        case OPTION_FILE:
            switch (_request.getRequestMethod()) {
                case POST:
                case PUT:{
//						std::cout << "analyse request: POST/PUT: trunc file\n";
                    std::fstream outFile;
                    outFile.open((_request.getFullPath()), std::ios::out | std::ios::trunc);
                    if(outFile.is_open())
                    {
                        _response.setFileIsFound(true);
                        outFile.close();
                    }
                    break;
                }
                case GET:
                case HEAD:{
//                        std::cout << _request.getFullPath() << "\n";
                    std::fstream inFile;
                    inFile.open((_request.getFullPath()),std::ios::in);
                    if(!inFile.is_open() || opendir(_request.getFullPath().c_str()))
                        std::cout << RED << _request.getFullPath() << " : error\n" << WHITE;
                    else{
                        _response.setFileIsFound(true);
                        inFile.close();
                    }
                    break;
                }
                case DELETE:{
                    std::fstream inFile;
                    inFile.open((_request.getFullPath()),std::ios::in);
                    if(!inFile.is_open() || opendir(_request.getFullPath().c_str()))
                        std::cout << RED << _request.getFullPath() << " : error\n" << WHITE;
                    else
                    {
                        _response.setFileIsFound(true);
                        std::remove(_request.getFullPath().c_str());
                        inFile.close();
                    }
                    break;
                }
                case NO_METHOD:
                case OTHER_METHOD:
                    _request.setRequestErrors(ERROR_REQUEST_NOT_VALID);
            }
            if(_request.getRequestOptionType() == OPTION_FILE && !_response.isFileIsFound())
            {
                _request.setRequestErrors(ERROR_FILE_NOT_FOUND);
            }
    }
}

void        Client::analysePath(){
    size_t pos;
    std::string fileName;
    std::string filePath;
    filePath = _request.getOption();
    std::vector<VirtualServerConfigDirectory>::const_reverse_iterator it = _serverConfig.getDirectories().rbegin();
    std::vector<VirtualServerConfigDirectory>::const_reverse_iterator itEnd = _serverConfig.getDirectories().rend();
    for(;it != itEnd; it++){
        if(filePath.find(it->getDirectoryName()) == 0)
        {
            _response.setPathIsAvailable(true);
            _request.setDirectoryConfig(*it);
            pos = it->getDirectoryAllowedMethods().find(_request.getType());
            if(pos != std::string::npos)
                _response.setMethodIsAllowed(true);
            if(!it->getDirectoryRedirect().empty() && it->getDirectoryName() == _request.getOption())
            {
//					std::cout << "redirects: " << it->getDirectoryRedirect() << " " << _request.getOption() << "\n";
                _request.setRedirect(it->getDirectoryRedirect());
                _request.setRequestErrors(ERROR_REDIRECT);
                return;
            }
            _request.setIsAutoIndex(it->isAutoindex());
            filePath.erase(0,it->getDirectoryName().size());
            filePath.insert(0,it->getDirectoryPath());
            _request.setFullPath(filePath);
            if(it->getMaxBodySize() >= 0)
                _request.setMaxBodySize(it->getMaxBodySize());
            break;
        }
    }
    // directory check

    struct stat s;
    if( stat(_request.getFullPath().c_str(),&s) == 0 && (s.st_mode & S_IFDIR))
    {
        _request.setRequestOptionType(OPTION_DIR);
        if (_request.getDirectoryConfig().getDirectoryIndexName().empty() && !_request.isAutoIndex() && _request.getRequestMethod() == GET){
            _request.setRequestErrors(ERROR_FILE_NOT_FOUND);
            return;
        }
    }

    // split to file and path
    pos = _request.getFullPath().find_last_of('/');
    if (pos != std::string::npos && (pos != _request.getFullPath().size() - 1 || pos == 0) && (_request.getRequestOptionType() != OPTION_DIR))
    {
        fileName = _request.getFullPath().substr(pos + 1, _request.getFullPath().size() - pos);
        filePath = _request.getFullPath().substr(0, pos + 1);
        _request.setOptionPath(filePath);
        _request.setOptionFileName(fileName);
        pos = fileName.find_last_of('.');
        if(pos != std::string::npos)
            _request.setOptionFileExtension(fileName.substr(pos + 1, fileName.size() - pos));
    }
    if((_request.getOptionFileExtension() == _request.getDirectoryConfig().getCgiExtention() && !_request.getOptionFileExtension().empty()))
        _response.setMethodIsAllowed(true);
}

void        Client::findVirtualServer()
{
    bool isFound = false;
    std::vector<VirtualServerConfig>::iterator def = _virtualServers.begin();
    std::vector<VirtualServerConfig>::iterator it = _virtualServers.begin();
    for(; it != _virtualServers.end(); it++)
    {
        if(it->getServerName() == _request.getHost())
        {
            setVirtualServerConfig(*it);
            isFound = true;
            break;
        }
    }
    if(!isFound)
    {
        setVirtualServerConfig(*def);
        std::cout << "default virtual server is set\n";
    }
}

bool        Client::isCgi()
{
    if ((_request.getOptionFileExtension() == _request.getDirectoryConfig().getCgiExtention()) && !_request.getDirectoryConfig().getCgiExtention().empty())
    {
        return true;
    }
    return false;
}