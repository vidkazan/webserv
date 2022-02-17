#pragma once
#include "main.hpp"
#include "AutoIndex.hpp"

class AutoIndex;
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
	void printRequestInfo(){
		std::cout << YELLOW;
		std::cout << "readBuffer: current parsed info:" << _socketFD <<"\n" \
						<<"type: "<< _request.getType() \
						<<"\n"<< _request.getOption() \
						<<"\nhttp: "<< _request.getHTTPVersion() \
						<<"\nhost: "<< _request.getHost() \
						<<"\ncontent-length: "<< _request.getContentLength() \
						<< std::endl;
		std::cout << WHITE;
	}
	int getStatus() const {return _status;};
	void resetData(){_status = READING;}
	const Response&getResponse() const{return _response;}
	const Request&getRequest() const{return _request;}
	int getSocketFd() const{return _socketFD;}
	void setStatus(int status){_status = status;};
	void setResponse(const Response&response){_response = response;}
	void readRequest()
	{
		std::ofstream file;
		file.open("tmp/log/fullReq_" + std::to_string(_request.getRequestId()) + ".txt", std::ios::app);
		recvBuffer(&file);
		if(_request.getBuffer().find("\r\n\r\n") != std::string::npos && _request.getReadStatus() == REQUEST_READ_WAITING_FOR_HEADER) {
			file << _request.getBuffer() << "\n";
			_request.setReadStatus(REQUEST_READ_HEADER);
		}
		if(_request.getReadStatus() == REQUEST_READ_HEADER)
			parseRequestHeader(&file);
		if(_request.getReadStatus() == REQUEST_READ_CHUNKED)
			parseRequestBodyChunked(&file);
		if(_request.getReadStatus() == REQUEST_READ_BODY)
			parseRequestBody(&file);
		if(_request.getReadStatus() == REQUEST_READ_MULTIPART)
			parseRequestMultiPart(&file);
		if(_request.getReadStatus() == REQUEST_READ_COMPLETE)
			_status = WRITING;
		file.close();
	}
	void recvBuffer(std::ofstream * file){
		ssize_t ret;
		char buf[100000];
		bzero(&buf, 100000);
		ret = recv(_socketFD, &buf, 99999, 0);
		if (ret == -1 || ret == 0){
			*file << "fd " << _socketFD << " status: closing\n";
			_status = CLOSING;
			return;
		}
//		if(_request.getType() == "POST")
//			*file << "\n>>>ret: " << ret << " buffer size: " << _request.getBuffer().size() << "\n";
		_request.appendBuffer(buf, ret);
//		if(_request.getType() == "POST")
//			*file<< "\n" << _request.getBuffer() << "\n\n"  << "buffer size: " << _request.getBuffer().size() << "\n";
	}
	void parseRequestHeader(std::ofstream * file)
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
				if (_request.getReadStatus() != REQUEST_READ_BODY && _request.getReadStatus() != REQUEST_READ_CHUNKED && _request.getReadStatus() != REQUEST_READ_MULTIPART){
					_request.setReadStatus(REQUEST_READ_COMPLETE);
					analyseRequest(file);
				}
				else if(_request.getReadStatus() == REQUEST_READ_BODY || _request.getReadStatus() == REQUEST_READ_CHUNKED || _request.getReadStatus() == REQUEST_READ_MULTIPART)
					analyseRequest(file);
				return;
			}
			pos = _request.getBuffer().find('\n');
			if(pos == std::string::npos)
				return;
			line = _request.getBuffer().substr(0,pos);
			if (_request.getType().empty() && !line.empty())
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
					if(_request.getType() == "POST")
						*file << "Request read status: REQUEST_READ_CHUNKED\n";
					_request.setReadStatus(REQUEST_READ_CHUNKED);
				}
			}
			else if(line.find("Content-Length: ") != std::string::npos){
				line.erase(0, 15);
				_request.setContentLength(std::stol(line));
				if(_request.getReadStatus() != REQUEST_READ_MULTIPART) {
					if(_request.getType() == "POST")
						*file << "Request read status: REQUEST_READ_BODY\n";
					_request.setReadStatus(REQUEST_READ_BODY);
				}
			}
			else if(line.find("X-Secret-Header-For-Test: 1") != std::string::npos){
//				if(line[line.size()-1] == '1')
					_request.setIsXSecretHeader(true);
				std::cout << "X-Secret-Header\n";
			}
			else if(line.find("Connection: close") != std::string::npos){
				std::cout << "Connection will be closed!\n";
				_response.setToCloseTheConnection(true);
			}
			else if(line.find("Content-Type: multipart/form-data; boundary=----WebKitFormBoundary") != std::string::npos)
			{
				_request.setIsMultiPart(true);
				_request.setReadStatus(REQUEST_READ_MULTIPART);
				*file << "Request read status: REQUEST_READ_MULTIPART\n";
			}
			tmp = _request.getBuffer();
			_request.setBuffer(tmp.erase(0, pos + 1));
		}
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
	void parseRequestBody(std::ofstream * file)
	{
		std::cout << "parseBody" << "\n";
		if(_request.getType() == "POST")
			*file << "Parse Body: " << "content length: " << _request.getContentLength() << " buffer size: " << _request.getBuffer().size() << "\n";
		_request.setContentLength(_request.getContentLength() - _request.getBuffer().size());
		if(!_request.isCgi())
		{
			std::ofstream outFile;
			outFile.open(_request.getFullPath(), std::ios::app);
			outFile << _request.getBuffer();
			outFile.close();
		}
		_request.setBuffer("");
		if(_request.getContentLength() == 0)
		{
//			std::cout << _request.getRequestId() << " request read status: REQUEST_READ_COMPLETE\n";
			_request.setReadStatus(REQUEST_READ_COMPLETE);
		}
	}
	void parseRequestMultiPart(std::ofstream * file)
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
		}
		else
		{
			_request.setContentLength(_request.getContentLength() - _request.getBuffer().size());
		}
		*file << "Parse Body: " << "content length: " << _request.getContentLength() << " buffer size: " << _request.getBuffer().size() << "\n";
		size_t pos = _request.getBuffer().find("------WebKitFormBoundary");
		if((pos != std::string::npos) && _request.getContentLength() == 0)
		{
			_request.setBuffer(_request.getBuffer().substr(0, pos - 1));
		}
		std::ofstream outFile;
		outFile.open(_request.getFullPath(), std::ios::app);
		outFile << _request.getBuffer();
		outFile.close();
		_request.setBuffer("");
		if(_request.getContentLength() == 0)
		{
//			std::cout << _request.getRequestId() << " request read status: REQUEST_READ_COMPLETE\n";
			_request.setReadStatus(REQUEST_READ_COMPLETE);
		}
	}
	void parseRequestBodyChunked(std::ofstream * file)
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
						*file << "chunk complete! chunkBufferSize:" << _request.getBufferChunk().size() << "\n" ;
						_request.setCounter(_request.getCounter() + _request.getBufferChunk().size());
						*file << "chunks: read: " << _request.getCounter() << " max body size: " << _request.getMaxBodySize() <<  "\n";
						if(_request.getMaxBodySize() >= 0 && ((ssize_t)_request.getCounter() > _request.getMaxBodySize()))
						{
								_request.setIsOverMaxBodySize(true);
							std::cout << RED << "OverMaxBobySize!" << WHITE << "\n";
						}
//						std::cout << "BufferSize:" << _request.getBuffer().size() << "\n";
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
			else // atatatatatatattttttaaaaaaaaaaa
				exit(2);
		}
	}
	void exportChunk(){
		if(!_request.isCgi())
		{
			std::ofstream outFile;
			outFile.open(_request.getFullPath(), std::ios::app);
			outFile << _request.getBufferChunk();
			outFile.close();
		}
		else
		{
			std::ofstream file;
			file.open("tmp/log/req_" + std::to_string(_request.getRequestId()) + ".txt", std::ios::app);
			file << _request.getBufferChunk();
			imitateCgi();
			file.close();
		}
	}

	void imitateCgi(){
			std::ofstream outFile;
			std::string in = _request.getBufferChunk();
			std::cout << "writing chunk to:" << _request.getFullPath() << "\n";
			outFile.open(_response.getCgiResFileName(), std::ios::app);
			for(size_t i=0;i < in.size(); i++)
			{
				if(_request.isXSecretHeader())
				{
					outFile << "1";
				}
				else
					outFile << (char)toupper(in[i]);
			}
			outFile.close();
	}
	void analyseRequest(std::ofstream * file){
		// check validness
		if(_request.getType().empty() || _request.getOption().empty() || _request.getHTTPVersion().empty() || _request.getHost().empty() )
		{
			_response.setRequestIsValid(false);
			std::cout << GREEN << "request isn't valid!\n" << WHITE;
			return;
		}
		analysePath(file);
		if(!_response.isMethodIsAllowed() || !_response.isPathIsAvailable())
			return;
		if(_request.getOptionFileExtension() == "bla"){
			_request.setIsCgi(true);
			*file << "is CGI\n";
		}
		if(_request.isCgi()){
			std::stringstream fileName;
			std::ofstream file;
			fileName << "tmp/tmp_";
			fileName << _socketFD;
			_response.setCgiResFileName(fileName.str());
			file.open(_response.getCgiResFileName(), std::ios::trunc);
			if(!file.is_open())
				std::cout << _response.getCgiResFileName() << " analyse request: cgi: file open error\n";
			return;
		}
		if((_request.getType() == "PUT" || _request.getType() == "POST") && !_request.isDirectory() && !_request.isCgi())
		{
			std::cout << "analyse request: POST/PUT: trunc file\n";
			std::fstream outFile;
			outFile.open((_request.getFullPath()), std::ios::out | std::ios::trunc);
			if(!outFile.is_open())
			{
				std::cout << _request.getFullPath() << " : error\n";
			}
			else
			{
				_response.setFileIsFound(true);
				outFile.close();
			}
		}

		if(_request.getType() == "GET" && !_request.isDirectory() && !_request.isCgi())
		{
			std::fstream inFile;
			inFile.open((_request.getFullPath()),std::ios::in);
			if(!inFile.is_open())
				std::cout << _request.getFullPath() << " : error\n";
			else
			{
				_response.setFileIsFound(true);
				inFile.close();
			}
		}
		if(_request.getType() == "DELETE")
		{
			std::fstream inFile;
			inFile.open((_request.getFullPath()),std::ios::in);
			if(!inFile.is_open())
				std::cout << _request.getFullPath() << " : error\n";
			else
			{
				_response.setFileIsFound(true);
				std::remove(_request.getFullPath().c_str());
				inFile.close();
			}
		}
	}
	void analysePath(std::ofstream * file){
		// FILE SEARCHING

		// 2. Search path in config
		// 3. Split filename to name and extension

		size_t pos;
		std::string fileName;
		std::string filePath;
		filePath = _request.getOption();
		std::vector<ListenSocketConfigDirectory>::const_reverse_iterator it = _serverConfig.getDirectories().rbegin();
		std::vector<ListenSocketConfigDirectory>::const_reverse_iterator itEnd = _serverConfig.getDirectories().rend();
		for(;it != itEnd; it++){
			if(filePath.find(it->getDirectoryName()) == 0)
			{
				*file << "path found in config: " << it->getDirectoryName() << "\n";
				_response.setPathIsAvailable(true);
				pos = it->getDirectoryAllowedMethods().find(_request.getType());
				if(pos != std::string::npos || (_request.getType() == "POST" && _request.getOptionFileExtension() == "bla")){
					*file << "Method is allowed\n";
					_response.setMethodIsAllowed(true);
				}
				filePath.erase(0,it->getDirectoryName().size());
				filePath.insert(0,it->getDirectoryPath());
				if(_request.getOption() == "/" && _request.getType() == "GET")   // костыыыыыыыль
					_request.setFullPath("www/index.html");
				else
					_request.setFullPath(filePath);
				*file << "for this dir maxBosySize: " << it->getMaxBodySize() << "\n";
				if(it->getMaxBodySize() >= 0)
				{
					std::cout << "MAX Body Size! "<< it->getMaxBodySize() << "\n";
					_request.setMaxBodySize(it->getMaxBodySize());
				}
				break;
			}
		}
		// directory check
		if(_request.getOption() != "/")
		{
			struct stat s;
			if( stat(_request.getFullPath().c_str(),&s) == 0 && (s.st_mode & S_IFDIR))
			{
				_request.setIsDirectory(true);
				*file << "analyse request: file is DIRECTORY\n";
			}
		}
		// split to file and path
		pos = _request.getFullPath().find_last_of('/');
		if (pos != std::string::npos && (pos != _request.getFullPath().size() - 1 || pos == 0) && !_request.isDirectory())
		{
			fileName = _request.getFullPath().substr(pos + 1, _request.getFullPath().size() - pos);
			filePath = _request.getFullPath().substr(0, pos + 1);
			_request.setOptionPath(filePath);
			_request.setOptionFileName(fileName);
			pos = fileName.find_last_of('.');
			if(pos != std::string::npos)
				_request.setOptionFileExtension(fileName.substr(pos + 1, fileName.size() - pos));
		}
		if(_request.getType() == "POST" && _request.getOptionFileExtension() == "bla")
			_response.setMethodIsAllowed(true);
	}

	void generateResponse()
	{
		static int counterForFile = 0;
		std::fstream inputFile;
		std::string bufResp;
		std::string body;

//		std::cout << "| Path is aval: " << _response.isPathIsAvailable() << " |\n";
//		std::cout << "| inFile is found: " << _response.isFileIsFound() << " |\n";
//		std::cout << "| is CGI: " << _request.isCgi() << " |\n";
		if(!_response.isRequestIsValid())
			bufResp += "HTTP/1.1 400 Bad Request\n";
		else if(!_response.isPathIsAvailable() || \
			(!_response.isFileIsFound() && !_request.getOptionFileName().empty() && !_request.isDirectory() && !_request.isCgi()) || \
			(_request.isDirectory() && (_request.getOption().find("Yeah") != std::string::npos)))
			bufResp = "HTTP/1.1 404 Not found\n";
		else if(!_response.isMethodIsAllowed())
			bufResp = "HTTP/1.1 405 Method Not Allowed\n";
		else if(_request.isOverMaxBodySize())
			bufResp = "HTTP/1.1 413 Payload Too Large\n";
		else
			bufResp = "HTTP/1.1 200 OK\r\n";

		if((_request.getType() == "GET" && !_request.isCgi()) || _request.getType() == "HEAD")
		{
			if (bufResp.find("400") != std::string::npos)
				inputFile.open("www/400.html", std::ios::in);
			if (bufResp.find("404") != std::string::npos)
				inputFile.open("www/404.html", std::ios::in);
			if (bufResp.find("405") != std::string::npos)
				inputFile.open("www/405.html", std::ios::in);
			if (bufResp.find("200") != std::string::npos && _request.isDirectory())
				inputFile.open("www/isDirectory.html", std::ios::in);
			if (bufResp.find("200") != std::string::npos)
				inputFile.open(_request.getFullPath(), std::ios::in);
			std::stringstream buffer;
			buffer << inputFile.rdbuf();
			body = buffer.str();
			if (bufResp.find("404") != std::string::npos)
				body = AutoIndex::generateAutoindexPage();
			//std::cout << inputFile;
			//std::cout << body << "\n";
			bufResp += "Content-Length: ";
			bufResp += std::to_string((unsigned  long long )body.size());
			bufResp +="\n";
			bufResp += "Content-Type: ";
//			std::cout << "genResp: path: " << _request.getFullPath() << "\n";
			//choosing type
			if(_request.getOptionFileExtension() == "html")
				bufResp += "text/html";
			if(_request.getOptionFileExtension() == "png")
				bufResp += "image/png";
		}
		else
		{
			if(_request.isCgi() && _request.getType() == "POST" && !_request.isOverMaxBodySize())
			{
				body = readCgiRes();
			}
			else if(_request.isMultiPart())
			{
				inputFile.open("www/uploadSuccess.html", std::ios::in);
				std::stringstream buffer;
				buffer << inputFile.rdbuf();
				body = buffer.str();
				body.replace(body.find("/fnm/"),5,_request.getMultiPartFileName());
			}
			bufResp += "Content-Length: ";
			bufResp += std::to_string((unsigned  long long )body.size());
		}
		bufResp += "\n\n";
		if(_request.getType() != "HEAD" && !body.empty())
			bufResp += body;
		allocateResponse(bufResp);
		std::ofstream logfile;
		logfile.open("tmp/log/resp_" + std::to_string(_request.getRequestId()) + ".txt", std::ios::trunc);
		logfile << bufResp;
		logfile.close();
		counterForFile++;
//		_response.setResponse(response);
		_status = WRITING;
		inputFile.close();
		Request request;
		_request = request;
	}

	void allocateResponse(std::string bufResp){
		char *res;
		size_t i=0;
		size_t size = bufResp.size();
		res = (char *)malloc(sizeof (char) * (size));
		for(;i < size;i++){
			res[i] = bufResp[i];
		}
		_response.setResponse(res,i);
	}
	void sendResponse()
	{
//		std::cout << "sending\n";
//				printLog(nullptr,getResponse().getResponse(), GREEN);

		ssize_t ret = send(_socketFD, _response.getResponse() + _response.getBytesSent(),_response.getResponseSize() - _response.getBytesSent(),0); //  SIGPIPE ignore
		if(ret <= 0)
		{
			setStatus(CLOSING);
			return;
		}
		_response.addBytesSent(ret);
//		std::cout << "sent: " << _response.getBytesSent() << "/ size: " << _response.getResponseSize() <<  "\n";
		if(_response.getBytesSent() == (size_t)_response.getResponseSize())
		{
			if(_response.toCloseTheConnection())
				setStatus(CLOSING);
			else
				setStatus(READING);
			Response response;
			setResponse(response);
		}
	}

	std::string readCgiRes(){
		std::ifstream file;
		file.open(_response.getCgiResFileName(), (std::ios_base::openmode)0);
		if(!file.is_open())
			std::cout << "readCgiRes: file error\n";
		std::stringstream str;
		str << file.rdbuf();
		return (str.str());
	}
};

