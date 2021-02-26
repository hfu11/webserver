#ifndef HTTPCONN_H
#define HTTPCONN_H

#include <sys/types.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "../buffer/Buffer.h"

class HttpConn
{
public:
    HttpConn(/* args */);
    ~HttpConn();

    void init(int sockFd, const sockaddr_in& sin);

    ssize_t read(int* saveError);

    ssize_t write(int* saveError);

    void Close();

    int GetFd() const;

    int GetPort() const;

    const char* GetIP() const;

    sockaddr_in GetAddr() const;

    bool process();

    int ToWriteBytes(){
        return iov_m[0].iov_len + iov_m[1].iov_len;
    }

    bool IsKeepAlive() const{
        return request_m.IsKeepAlive();
    }

    static bool isET;
    static const char* srcDir;
    static std::atomic<int> userCount;

private:
    int fd_m;
    sockaddr_in addr_m;
    
    bool isClose_m;
    int iovCnt_m;
    iovec iov_m[2];

    Buffer readBuff_m;
    Buffer writeBuff_m;

    HttpRequest request_m;
    HttpResponse response_m;
};

#endif

