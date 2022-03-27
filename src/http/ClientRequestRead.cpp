//
// Created by Dmitrii Grigorev on 27.03.22.
//

#include "../main.hpp"

void        Client::readRequest()
{
    std::ofstream file;
    file.open("tmp/log/fullReq_" + std::to_string(_request.getRequestId()) + ".txt", std::ios::app);
    recvBuffer();

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
                parseRequestBody();
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
void        Client::recvBuffer(){
    ssize_t ret;
    char buf[100000];
    bzero(&buf, 100000);
    ret = recv(_socketFD, &buf, 99999, 0);
    if (ret == -1 || ret == 0)
    {
        free(_response.getResponse());
        _status = CLOSING;
        return;
    }

    _request.appendBuffer(buf, ret);
}