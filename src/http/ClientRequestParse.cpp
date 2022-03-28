//
// Created by Dmitrii Grigorev on 27.03.22.
//
#include "../main.hpp"

void        Client::parseRequestHeader(std::ofstream * file)
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
            analyseRequest();
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
void        Client::parseRequestTypeOptionVersion(std::string str)
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
void        Client::parseRequestBody()
{
    if(_request.getMaxBodySize() >= 0 && (_request.getContentLength() > _request.getMaxBodySize())) {
        _request.setRequestErrors(ERROR_BODY_OVER_MAX_SIZE);
        return;
    }
    _request.setCounter(_request.getCounter() + _request.getBuffer().size());
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
    if(_request.getContentLength() > -1 && ((size_t)_request.getContentLength() <= _request.getCounter()))
    {
//			std::cout << _request.getRequestId() << " request read status: REQUEST_READ_COMPLETE\n";
        _request.setReadStatus(REQUEST_READ_COMPLETE);
    }
}
void        Client::parseRequestMultiPart()
{
    if(_request.getMaxBodySize() >= 0 && (_request.getContentLength() > _request.getMaxBodySize())) {
        _request.setRequestErrors(ERROR_BODY_OVER_MAX_SIZE);
        return;
    }
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
        _request.setContentLength(_request.getContentLength() - _request.getBuffer().size());
    size_t pos = _request.getBuffer().find("------WebKitFormBoundary");

    if((pos != std::string::npos) && _request.getContentLength() == 0)
        _request.setBuffer(_request.getBuffer().substr(0, pos - 1));
    std::ofstream outFile;
    outFile.open(_request.getFullPath(), std::ios::app);
    outFile << _request.getBuffer();
    outFile.close();
    _request.setCounter(_request.getCounter() + _request.getBuffer().size());
    _request.setBuffer("");

    if(_request.getContentLength() == 0){
        _request.setReadStatus(REQUEST_READ_COMPLETE);
    }
}
void        Client::parseRequestBodyChunked(std::ofstream * file)
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
//            *file << "new chunk size DEC: " << tmpChunkSize << "\n";
            _request.setChunkSize(tmpChunkSize);
            tmp.erase(0,pos + 2);
            _request.setBuffer(tmp);
        }
        if(_request.getChunkSize() != -1)
        {
//            *file << "current chunkSize: " << _request.getChunkSize() << " bufferSize: " << tmp.size() << "\n";
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
//                    *file << "written:" << _request.getCounter() << "\n";
//                    *file << _request.getRequestId() << " request read status: REQUEST_READ_COMPLETE\n";
                }
                else
                {
                    // chunk complete
                    // export chunkBuffer
//                    *file << "chunk complete! chunkBufferSize:" << _request.getBufferChunk().size() << "\n" ; // FIXME maxBodySize made only for chunks!
                    _request.setCounter(_request.getCounter() + _request.getBufferChunk().size());
//                    *file << "chunks: read: " << _request.getCounter() << " max body size: " << _request.getMaxBodySize() <<  "\n";
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
void        Client::exportChunk(){
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
