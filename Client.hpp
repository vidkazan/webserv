#pragma once
#include "main.hpp"
#include "AutoIndex.hpp"

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
	void        readRequest()
	{
        std::ofstream file;
        file.open("tmp/log/fullReq_" + std::to_string(_request.getRequestId()) + ".txt", std::ios::app);
		recvBuffer(&file);
    
		int previousReadStatus = -1;
		while(previousReadStatus != _request.getReadStatus())
		{
			previousReadStatus = _request.getReadStatus();
			switch (_request.getReadStatus()) {
				case REQUEST_READ_WAITING_FOR_HEADER:
					if (_request.getBuffer().find("\r\n\r\n") != std::string::npos) {
						file << _request.getBuffer() << "\n";
						_request.setReadStatus(REQUEST_READ_HEADER);
						parseRequestHeader(&file);
					}
					break;
				case REQUEST_READ_HEADER:
					parseRequestHeader(&file);break;
				case REQUEST_READ_CHUNKED:
					parseRequestBodyChunked(&file);
					_request.setRequestBodyType(BODY_CHUNKED);
					break;
				case REQUEST_READ_BODY:
					parseRequestBody(&file);
					_request.setRequestBodyType(BODY_BASE);
					break;
				case REQUEST_READ_MULTIPART:
					parseRequestMultiPart(&file);
					_request.setRequestBodyType(BODY_MULTI_PART);
					break;
				case REQUEST_READ_COMPLETE:
					_status = WRITING; break;
			}
		}
		file.close();
	}
	void        recvBuffer(std::ofstream * file){
		ssize_t ret;
		char buf[100000];
		bzero(&buf, 100000);
		ret = recv(_socketFD, &buf, 99999, 0);
        if(ret < 1)
        {
            std::cout << "fd: " << _socketFD << " recv: " << ret << "\n";
        }
		if (ret == -1 || ret == 0)
        {
            free(_response.getResponse());
			_status = CLOSING;
			return;
		}

		_request.appendBuffer(buf, ret);
	}
	void        parseRequestHeader(std::ofstream * file)
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

                if(_request.getReadStatus() == REQUEST_READ_HEADER)
                    _request.setReadStatus(REQUEST_READ_COMPLETE);
				analyseRequest(file);
				return;
			}
			if((pos = _request.getBuffer().find("\n")) == std::string::npos)
				return;
			line = _request.getBuffer().substr(0,pos - 1); // (pos -1):  -1 is \r
			if (_request.getRequestMethod() == NO_METHOD && !line.empty())
				parseRequestTypeOptionVersion(line);
			else if(line.find("Host: ") != std::string::npos){
				line.erase(0, 6);
				_request.setHost(line);
			}
			else if(line.find("Transfer-Encoding: ") != std::string::npos)
			{
				line.erase(0, 19);
				if (line.find("chunked") != std::string::npos)
				{
					if(_request.getRequestMethod() == POST)
						*file << "Request read status: REQUEST_READ_CHUNKED\n";
					_request.setReadStatus(REQUEST_READ_CHUNKED);
					_request.setRequestBodyType(BODY_CHUNKED);
				}
			}
			else if(line.find("Content-Length: ") != std::string::npos){
				line.erase(0, 15);
				_request.setContentLength(std::stol(line));
				if(_request.getReadStatus() != REQUEST_READ_MULTIPART) {
					_request.setReadStatus(REQUEST_READ_BODY);
					_request.setRequestBodyType(BODY_BASE);
				}
			}
            else if(line.find("X-Secret-Header-For-Test: 1") != std::string::npos){
//				if(line[line.size()-1] == '1')
                _request.setIsXSecretHeader(true);
//                std::cout << "X-Secret-Header\n";
            }
			else if(line.find("Connection: close") != std::string::npos){
//				std::cout << "Connection will be closed!\n";
				_response.setToCloseTheConnection(true);
			}
			else if(line.find("Content-Type: multipart/form-data; boundary=----WebKitFormBoundary") != std::string::npos)
			{
				_request.setReadStatus(REQUEST_READ_MULTIPART);
				_request.setRequestBodyType(BODY_MULTI_PART);
				*file << "Request read status: REQUEST_READ_MULTIPART\n";
			}
			tmp = _request.getBuffer();
			_request.setBuffer(tmp.erase(0, pos + 1));
		}
	}
	void        parseRequestTypeOptionVersion(std::string str)
	{
		size_t pos = str.find(' ');
		if(pos != std::string::npos) {
			_request.setType(str.substr(0, pos));
			str.erase(0,pos+1);
			if(_request.getType() == "GET")
				_request.setRequestMethod(GET);
			else if(_request.getType() == "POST")
				_request.setRequestMethod(POST);
			else if(_request.getType() == "PUT")
				_request.setRequestMethod(PUT);
			else if(_request.getType() == "DELETE")
				_request.setRequestMethod(DELETE);
			else if(_request.getType() == "HEAD")
				_request.setRequestMethod(HEAD);
			else
				_request.setRequestMethod(OTHER_METHOD);
		}
		pos = str.find(' ');
		if(pos != std::string::npos){
			_request.setOption(str.substr(0, pos));
			str.erase(0,pos+1);
		}
		if(!str.empty())
			_request.setHTTPVersion(str);
	}
	void        parseRequestBody(std::ofstream * file)
	{
		if(_request.getRequestMethod() == POST) {
//			std::cout << "parse Body: " << "content length: " << _request.getContentLength() << " buffer size: " << _request.getBuffer().size() << " buffer: " << _request.getBuffer() << "\n";
		}
		_request.setContentLength(_request.getContentLength() - _request.getBuffer().size());
		if(_request.getRequestOptionType() != OPTION_CGI)
		{
			std::ofstream outFile;
			outFile.open(_request.getFullPath(), std::ios::app);
			outFile << _request.getBuffer();
			outFile.close();
		}
        if(_request.getRequestOptionType() == OPTION_CGI)
        {
            std::ofstream outFile;
            outFile.open(_response.getCgiInputFileName(), std::ios::app);
            outFile << _request.getBuffer();
            outFile.close();
        }
		_request.setBuffer("");
		if(_request.getContentLength() <= 0)
		{
//			std::cout << _request.getRequestId() << " request read status: REQUEST_READ_COMPLETE\n";
			_request.setReadStatus(REQUEST_READ_COMPLETE);
		}
	}
	void        parseRequestMultiPart(std::ofstream * file)
	{
		if(_request.getMultiPartFileName().empty())
		{
			size_t pos = _request.getBuffer().find("\r\n\r\n");
			if(pos == std::string::npos)
				return;
			_request.setContentLength(_request.getContentLength() - _request.getBuffer().size());
			std::string header = _request.getBuffer().substr(0, pos);
			_request.setBuffer(_request.getBuffer().substr(pos + 4, _request.getBuffer().size() - 1));
			pos = header.find("filename=\"");
			header = header.substr(pos + 10, header.size() - 1);
			pos = header.find('\"');
			_request.setMultiPartFileName(header.substr(0,pos));
			_request.setFullPath(_request.getFullPath() + _request.getMultiPartFileName());
//			std::cout << "212 " << _request.getFullPath() << " + " << _request.getMultiPartFileName() << "\n";
		}
		else {
			_request.setContentLength(_request.getContentLength() - _request.getBuffer().size());
		}
		*file << "Parse Body: " << "content length: " << _request.getContentLength() << " buffer size: " << _request.getBuffer().size() << "\n";
		size_t pos = _request.getBuffer().find("------WebKitFormBoundary");

		if((pos != std::string::npos) && _request.getContentLength() == 0){
			_request.setBuffer(_request.getBuffer().substr(0, pos - 1));
		}
		std::ofstream outFile;
		outFile.open(_request.getFullPath(), std::ios::app);
		outFile << _request.getBuffer();
		outFile.close();
		_request.setBuffer("");

		if(_request.getContentLength() == 0){
			_request.setReadStatus(REQUEST_READ_COMPLETE);
		}
	}
	void        parseRequestBodyChunked(std::ofstream * file)
	{
		std::string tmp;
		size_t pos;

		// taking read buffer
		tmp = _request.getBuffer();
		// while chunk is not complete
		while(_request.getBufferChunk().empty())
		{
			// if dont have chunk size(chunkSize -1)
			// get chunk size \r\n
			// erase size line from buffer

			// first chunk line must consist of "<HEX chunk size>\r\n"
			// if we are waiting the chunk size line, checking chunk size line is ready
			if(_request.getChunkSize() == -1)
			{
				if((pos = tmp.find("\r\n")) == std::string::npos)
					return;

				*file << "\n\nnew chunk size HEX: " << tmp.substr(0, pos) << "\n";
				// converting request chunk size from HEX-string to DEC-long
				ssize_t tmpChunkSize;
				std::istringstream(tmp.substr(0, pos)) >> std::hex >> tmpChunkSize;
				*file << "new chunk size DEC: " << tmpChunkSize << "\n";
				_request.setChunkSize(tmpChunkSize);
				tmp.erase(0,pos + 2);
				_request.setBuffer(tmp);
			}
			if(_request.getChunkSize() != -1)
			{
				*file << "current chunkSize: " << _request.getChunkSize() << " bufferSize: " << tmp.size() << "\n";
				if (tmp.size() >= static_cast<size_t>(_request.getChunkSize()) + 2){
					// get chunk body
					// set chunkBuffer
					_request.setBufferChunk(tmp.substr(0, _request.getChunkSize()));
					// remove \r\n
					tmp.erase(0,_request.getChunkSize());
					{
						tmp.erase(0, 2);
						_request.setBuffer(tmp);
					}
					if(_request.getChunkSize() == 0)
					{
						_request.setReadStatus(REQUEST_READ_COMPLETE);
						*file << "written:" << _request.getCounter() << "\n";
						*file << _request.getRequestId() << " request read status: REQUEST_READ_COMPLETE\n";
					}
					else
					{
						// chunk complete
						// export chunkBuffer
						*file << "chunk complete! chunkBufferSize:" << _request.getBufferChunk().size() << "\n" ; // FIXME maxBodySize made only for chunks!
						_request.setCounter(_request.getCounter() + _request.getBufferChunk().size());
						*file << "chunks: read: " << _request.getCounter() << " max body size: " << _request.getMaxBodySize() <<  "\n";
						if(_request.getMaxBodySize() >= 0 && ((ssize_t)_request.getCounter() > _request.getMaxBodySize()))
						{
								_request.setRequestErrors(ERROR_BODY_OVER_MAX_SIZE);
//							std::cout << RED << "OverMaxBobySize!" << WHITE << "\n";
						}
						exportChunk();
					}
					// clean chunkBuffer
					// clean chunkBufferSize
					_request.setBufferChunk("");
					_request.setChunkSize(-1);
				}
				else
					// return for new extended buffer
					return;
			}
		}
	}
	void        exportChunk(){
		switch (_request.getRequestOptionType()) {
			case OPTION_CGI:
			{
//				std::ofstream file;
//				file.open("tmp/log/req_" + std::to_string(_request.getRequestId()) + ".txt", std::ios::app);
//				file << _request.getBufferChunk();
                std::ofstream file;
                file.open(_response.getCgiInputFileName(), std::ios::app);
                file << _request.getBufferChunk();
				file.close();
				break;
			}
			case OPTION_FILE:
			case OPTION_DIR:
			{
				std::ofstream outFile;
				outFile.open(_request.getFullPath(), std::ios::app);
				outFile << _request.getBufferChunk();
				outFile.close();
				break;
			}
			case NO_OPTION:
				break;
		}
	}
    void        findVirtualServer()
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
	void        analyseRequest(std::ofstream * file)
	{
		if(_request.getRequestMethod() == NO_METHOD || _request.getOption().empty() || _request.getHTTPVersion().empty() || _request.getHost().empty() )
		{
			_response.setRequestIsValid(false);
			_request.setRequestErrors(ERROR_REQUEST_NOT_VALID);
			std::cout << GREEN << "request isn't valid!\n" << WHITE;
			return;
		}
		findVirtualServer();
		analysePath(file);

		if(!_response.isMethodIsAllowed()){
			_request.setRequestErrors(ERROR_METHOD_NOT_ALLOWED);
		}
		if(!_response.isPathIsAvailable()){
			_request.setRequestErrors(ERROR_PATH_NOT_AVAILABLE);
		}
		if (this->isCgi()) {
			_request.setRequestOptionType(OPTION_CGI);
		}
		switch (_request.getRequestErrors()) {
			case NO_ERROR:
				break;
			default:
				return;
		}
		switch (_request.getRequestOptionType()) {
			case OPTION_CGI:
            case OPTION_DIR:
				break;
			default:
				_request.setRequestOptionType(OPTION_FILE); // buggy place?
		}
		switch (_request.getRequestOptionType()) {
			case NO_OPTION:
			case OPTION_DIR:
				break;
			case OPTION_CGI: {
                if(_request.getRequestMethod() == POST && _request.getOptionFileExtension() == _request.getDirectoryConfig().getCgiExtention())
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
						if(!outFile.is_open()){
//							std::cout << _request.getFullPath() << " : error\n";
						}
						else{
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
	bool        isCgi() {
        if ((_request.getOptionFileExtension() == _request.getDirectoryConfig().getCgiExtention()) && !_request.getDirectoryConfig().getCgiExtention().empty())
		{
				return true;
		}
		return false;
	}
	void        analysePath(std::ofstream * file){
		size_t pos;
		std::string fileName;
		std::string filePath;
		filePath = _request.getOption();
		std::vector<VirtualServerConfigDirectory>::const_reverse_iterator it = _serverConfig.getDirectories().rbegin();
		std::vector<VirtualServerConfigDirectory>::const_reverse_iterator itEnd = _serverConfig.getDirectories().rend();
		for(;it != itEnd; it++){
			if(filePath.find(it->getDirectoryName()) == 0)
			{
				*file << "path found in config: " << it->getDirectoryName() << "\n";
				_response.setPathIsAvailable(true);
                _request.setDirectoryConfig(*it);
				pos = it->getDirectoryAllowedMethods().find(_request.getType());

				if(pos != std::string::npos || \
                  (_request.getRequestMethod() == POST && \
                   _request.getOptionFileExtension() == _request.getDirectoryConfig().getCgiExtention()) && \
                   !_request.getOptionFileExtension().empty())
                {
					_response.setMethodIsAllowed(true);
				}
				// FIXME check redirections - tester not works with redirections
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
				*file << "for this dir maxBosySize: " << it->getMaxBodySize() << "\n";
				if(it->getMaxBodySize() >= 0)
				{
//					std::cout << "MAX Body Size! "<< it->getMaxBodySize() << "\n";
					_request.setMaxBodySize(it->getMaxBodySize());
				}
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

		if(_request.getRequestMethod() == POST && _request.getOptionFileExtension() == "bla")
			_response.setMethodIsAllowed(true);
	}
	void        generateResponse()
	{
        std::string tmp = std::to_string(getSocketFd()) + " " + _request.getOption();
		printStates(tmp);
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
//                                else
//                                    inputFile.open("www/isDirectory.html");
                                break;
                            }
							case OPTION_FILE: {
//								std::cout << "index name: " << _request.getFullPath() << "\n";
								inputFile.open(_request.getFullPath(), std::ios::in);
								break;
							}
                            case NO_OPTION:
                                break;
							case OPTION_CGI:
                            {
//								std::cout << "cgi--- " << _request.getFullPath() << "\n";
                                CGI *cgi = new CGI(_request.getType(), _request.getFullPath(), \
                                                    _response.getCgiOutputFileName(), _response.getCgiInputFileName());
                                try {
                                    cgi->executeCgiScript();
                                    // bufResp += cgi->getContentTypeStr();
                                    // bufResp += "\n";
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
				if(_request.getRequestOptionType() != OPTION_CGI) {
					std::stringstream buffer;
					buffer << inputFile.rdbuf();
					if (body.empty())
						body = buffer.str();
				}
					bufResp += "Content-Length: ";
					bufResp += std::to_string((unsigned long long) body.size());
					bufResp += "\n";
					bufResp += "Content-Type: ";
					//choosing type
					if (_request.getOptionFileExtension() == "html")
						bufResp += "text/html";
					if (_request.getOptionFileExtension() == "png")
						bufResp += "image/png";
				break;
			}
			case PUT:
			case POST:{
				switch (_request.getRequestOptionType()){
					case OPTION_CGI:
                        {
                            CGI *cgi = new CGI(_request.getType(), _request.getFullPath(), \
                                                    _response.getCgiOutputFileName(), _response.getCgiInputFileName());
                            try {
                                cgi->executeCgiScript();
//                                bufResp += cgi->getContentTypeStr();
//                                bufResp += "\n";
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
				bufResp += "Content-Length: ";
				bufResp += std::to_string((unsigned  long long )body.size());
			}
			case NO_METHOD:
			case DELETE:
			case OTHER_METHOD:
				break;
		}
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
	void        allocateResponse(std::string bufResp){
		char *res;
		size_t i=0;
		size_t size = bufResp.size();
		res = (char *)malloc(sizeof (char) * (size));
		for(;i < size;i++){
			res[i] = bufResp[i];
		}
		_response.setResponse(res,i);
//        std::cout << RED << "malloc ";
//        printf(" %p ", _response.getResponse());
//        std::cout <<  " " << getSocketFd() << WHITE << "\n";
	}
	void        sendResponse()
	{
//        if(_request.getRequestMethod() == POST)
//        {
//            std::string path = "tmp/logOutput/logOutput_" + std::to_string(_request.getRequestId()) + ".txt";
//            int fd = open(path.c_str(), O_CREAT | O_RDWR | O_APPEND);
//            write(fd, _response.getResponse() + _response.getBytesSent(),
//                  _response.getResponseSize() - _response.getBytesSent()); //  SIGPIPE ignore
//            close(fd);
//        }
		ssize_t ret = send(_socketFD, _response.getResponse() + _response.getBytesSent(),_response.getResponseSize() - _response.getBytesSent(),0); //  SIGPIPE ignore
        if(ret < 1)
        {
            std::cout << "fd: " << _socketFD << "send: " << ret << "\n";
        }
        if(ret <= 0)
		{
            free(_response.getResponse());
			setStatus(CLOSING);
			return;
		}
		_response.addBytesSent(ret);
//        std::cout << YELLOW << "sent ";
//        printf(" %p ", _response.getResponse());
//        std::cout <<  " " << getSocketFd() << ": " << _response.getBytesSent() << WHITE << "\n";
		if(_response.getBytesSent() == (size_t)_response.getResponseSize())
		{
            std::cout << GREEN << _request.getRequestId() << " " << getSocketFd() << " " << "sent: " << _response.getBytesSent() << WHITE << "\n";
			if(_response.toCloseTheConnection())
				setStatus(CLOSING);
			else
				setStatus(READING);
            free(_response.getResponse());
//            std::cout << YELLOW << "free ";
//            printf(" %p ", _response.getResponse());
//            std::cout <<  " " << getSocketFd() << WHITE << "\n";
			Response response;
			setResponse(response);
            Request request;
            _request = request;
		}
	}

	void        printStates(std::string place){
		std::cout << "| RS:" << _request.getReadStatus() << " | M:" << _request.getRequestMethod() << " | B:" << _request.getRequestBodyType() <<  " | O:" << _request.getRequestOptionType() << " | E:" << _request.getRequestErrors() << " |"  << place << "\n";;
	}
};

