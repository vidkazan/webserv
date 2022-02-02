#pragma once
#include "main.hpp"

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
		ssize_t ret;
		char buf[100000];
		bzero(&buf, 100000);
		ret = recv(_socketFD, &buf, 100000, 0);
		if (ret == -1 || ret == 0){
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
			std::cout << YELLOW;
			std::cout << "current parsed info:" << _socketFD <<"\n" \
						<<"type: "<< _request.getType() \
						<<"\n"<< _request.getOption() \
						<<"\nhttp: "<< _request.getHTTPVersion() \
						<<"\nhost: "<< _request.getHost() \
						<<"\ncontent-length: "<< _request.getContentLength() \
						<< std::endl;
						std::cout << WHITE;
		}
		if(_request.getReadStatus() == REQUEST_READ_BODY)
		{
			
		}
		else if(_request.getReadStatus() == REQUEST_READ_CHUNKED)
			parseRequestBodyChunked();
		if(_request.getReadStatus() == REQUEST_READ_COMPLETE) {
			_status = WRITING;
		}
	}

	void	allocateResponse(std::string bufResp){
		char *res;
		size_t i=0;
		size_t size = bufResp.size();
		res = (char *)malloc(sizeof (char) * (size));
		if(!res)
			exit(3);
		for(;i < size;i++){
			res[i] = bufResp[i];
		}
		_response.setResponse(res,i);
	}

	void generateResponse()
	{
		std::fstream inputFile;
		std::string bufResp;
		std::string body;
		std::string ext;
		size_t pos;
		if(!_response.isRequestIsValid())
			bufResp += "HTTP/1.1 400 Bad Request\n";
		else if(!_response.isPathIsAvailable() || !_response.isFileIsFound())
			bufResp = "HTTP/1.1 404 Not found\n";
		else if(!_response.isMethodIsAllowed())
			bufResp = "HTTP/1.1 405 Method Not Allowed\n";
		else
			bufResp = "HTTP/1.1 200 OK\n";

		if(bufResp.find("400") != std::string::npos)
			inputFile.open("www/400.html",std::ios::in);
		if(bufResp.find("404") != std::string::npos)
			inputFile.open("www/404.html",std::ios::in);
		if(bufResp.find("405") != std::string::npos)
			inputFile.open("www/405.html",std::ios::in);
		if(bufResp.find("200") != std::string::npos)
			inputFile.open(_response.getPath(),std::ios::in);
//		for (std::string line; std::getline(inputFile, line); ) {
//			body += line;
//		}

		std::stringstream buffer;
		buffer << inputFile.rdbuf();
		body = buffer.str();
//		std::cout << inputFile;
//		std::cout << body << "\n";
		bufResp += "Content-Length: ";
		bufResp += std::to_string((unsigned  long long )body.size());
		bufResp +="\n";
		bufResp += "Content-Type: ";
//		choosing type

		std::cout << "path: " << _response.getPath() << "\n";
		pos = _response.getPath().find_last_of('.');
		if(pos != std::string::npos)
		{
			ext = _response.getPath().substr(pos + 1, _response.getPath().size() - pos);
			std::cout << "extension: " << ext << "\n";
		}
		if(ext == "html")
			bufResp += "text/html";
		if(ext == "png")
			bufResp += "image/png";
		bufResp += "\n\n";
		bufResp += body;
//		std::cout << GREEN << bufResp << WHITE;
		allocateResponse(bufResp);
//		_response.setResponse(response);
		_status = WRITING;
		inputFile.close();
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
		size_t pos;
		std::string fileName;
		std::string filePath;
		// FILE SEARCHING

		// client: option: "/dir1/ + fileName"
		// server: config: "/dir1/ -> www/dir1/"

		if(_request.getType().empty() || _request.getOption().empty() || _request.getHTTPVersion().empty() || _request.getHost().empty() )
		{
			_response.setRequestIsValid(false);
			return;
		}
		//split option to path and filename
		pos = _request.getOption().find_last_of('/');
		if(pos != std::string::npos)
		{
			// split option by / ???
			fileName = _request.getOption().substr(pos + 1, _request.getOption().size() - pos);
			filePath = _request.getOption().substr(0, pos + 1);
			std::cout << "filename: " << fileName << " | filepath: " << filePath << "\n";
		}
		std::vector<ListenSocketConfigDirectory>::const_iterator it = _serverConfig.getDirectories().begin();
		std::vector<ListenSocketConfigDirectory>::const_iterator itEnd = _serverConfig.getDirectories().end();
		for(;it != itEnd; it++){
			if(it->getDirectoryName() == filePath)
			{
				_response.setPathIsAvailable(true);
				pos = it->getDirectoryAllowedMethods().find(_request.getType());
				if(pos != std::string::npos)
					_response.setMethodIsAllowed(true);
					if(filePath == "/" && fileName.empty())   // костыыыыыыыль
						_response.setPath("www/index.html");
					else
						_response.setPath(it->getDirectoryPath());
			}
		}
		if((_request.getType() == "PUT" || _request.getType() == "POST") && _response.isMethodIsAllowed() && _response.isPathIsAvailable() && _response.isRequestIsValid())
		{
			std::fstream outFile;
			outFile.open((_response.getPath() + fileName),std::ios::out | std::ios::trunc);
			if(!outFile.is_open())
			{
				std::cout << _response.getPath() << fileName << " : error\n";
			}
			else
			{
				_response.setFileIsFound(true);
				_response.setPath(_response.getPath() + fileName);
				outFile.close();
			}
		}
		if(_request.getType() == "GET" && _response.isMethodIsAllowed() && _response.isPathIsAvailable() && _response.isRequestIsValid())
		{
			std::fstream inFile;
			inFile.open((_response.getPath() + fileName),std::ios::in);
			if(!inFile.is_open())
			{
				std::cout << _response.getPath() << fileName << " : error\n";
			}
			else
			{
				_response.setFileIsFound(true);
				_response.setPath(_response.getPath() + fileName);
				inFile.close();
			}
		}
	}

	void parseRequestBody(){}

	void parseRequestBodyChunked()
	{
		std::string tmp;
		size_t pos;

		// taking read buffer
		tmp = _request.getBuffer();
//		printLog("requestBuffer:", (char *)tmp.c_str(),RED);

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

//				std::cout << "\n\nnew chunk size HEX: " << tmp.substr(0, pos) << "\n";
				// converting request chunk size from HEX-string to DEC-long
				ssize_t tmpChunkSize;
				std::istringstream(tmp.substr(0, pos)) >> std::hex >> tmpChunkSize;
//				std::cout << "new chunk size DEC: " << tmpChunkSize << "\n";
				_request.setChunkSize(tmpChunkSize);
				tmp.erase(0,pos + 2);
				_request.setBuffer(tmp);
			}
			if(_request.getChunkSize() != -1)
			{
//				std::cout << "current chunkSize: " << _request.getChunkSize() << " bufferSize: " << tmp.size() << "\n";

				// if buffer size >= chunkSize + 2(\r\n)
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
					if(_request.getChunkSize() == 0){
//						_request.setLastChunkStatus(true);
						_request.setReadStatus(REQUEST_READ_COMPLETE);
						std::cout <<  "written:" << _request.getCounter() << "\n";
						std::cout << "Request read status: REQUEST_READ_COMPLETE\n";
					}
					else
					{
						// chunk complete
						// export chunkBuffer
//						std::cout << "chunk complete! chunkBufferSize:" << _request.getBufferChunk().size() << "\n" ;
						_request.setCounter(_request.getCounter() + _request.getBufferChunk().size());
//						std::cout << "BufferSize:" << _request.getBuffer().size() << "\n" ;
						exportChunk();
					}
					// clean chunkBuffer
					// clean chunkBufferSize
					_request.setBufferChunk("");
					_request.setChunkSize(-1);
//					printLog("requestBuffer:", (char *)_request.getBuffer().c_str(),RED);
				}
				else
					// return for new extended buffer
					return;
			}
			else // atatatatatatattttttaaaaaaaaaaa
				exit(2);
		}
	}

	void exportChunk(){
		std::ofstream outFile;
//		std::cout << "writing chunk to:" << _response.getPath() << "\n";
		outFile.open(_response.getPath(), std::ios::app);
		outFile << _request.getBufferChunk();
		outFile.close();
	}

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
				if (_request.getReadStatus() != REQUEST_READ_BODY && _request.getReadStatus() != REQUEST_READ_CHUNKED){
					_request.setReadStatus(REQUEST_READ_COMPLETE);
					analyseRequest();
				}
				else if(_request.getReadStatus() == REQUEST_READ_BODY || _request.getReadStatus() == REQUEST_READ_CHUNKED)
					analyseRequest();
//				printLog("requestBuffer:", (char *)_request.getBuffer().c_str(),RED);
				return;
			}
			pos = _request.getBuffer().find('\n'); // pos check??
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

	void sendResponse()
	{
		std::cout << "sending\n";
//				printLog(nullptr,it->getResponse().getResponse(), GREEN);

		ssize_t ret = send(getSocketFd(), _response.getResponse() + _response.getBytesSent(),_response.getResponseSize() - _response.getBytesSent(), MSG_NOSIGNAL); //  SIGPIPE ignore
		if(ret <= 0)
		{
			setStatus(CLOSING);
			return;
		}
		_response.addBytesSent(ret);
		std::cout << "sent: " << _response.getBytesSent() << "/ size: " << _response.getResponseSize() <<  "\n";
		if(_response.getBytesSent() == (size_t)_response.getResponseSize())
		{
			Response response;
			setResponse(response);
			setStatus(READING);
		}
	}
};