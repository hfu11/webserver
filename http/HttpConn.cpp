#include "HttpConn.h"

using namespace std;

const char* HttpConn::srcDir;
atomic<int> HttpConn::userCount;
bool HttpConn::isET;

HttpConn::HttpConn(){
    fd_m = -1;
    addr_m = {0};
    isClose_m = true;
}

HttpConn::~HttpConn(){
    Close();
}

void HttpConn::init(int fd, const sockaddr_in& addr){
    assert(fd > 0);
    userCount++;
    addr_m = addr;
    fd_m = fd;
    //清空buffer
    writeBuff_m.RetrieveAll();  
    readBuff_m.RetrieveAll();
    isClose_m = false;
    //TODO日志
}

int HttpConn::GetFd() const{
    return fd_m;
}

sockaddr_in HttpConn::GetAddr() const{
    return addr_m;
}

const char* HttpConn::GetIP() const{
    return inet_ntoa(addr_m.sin_addr);
}

int HttpConn::GetPort() const{
    return addr_m.sin_port;
}

ssize_t HttpConn::read(int *saveError){
    ssize_t len = -1;
    do{
        len = readBuff_m.ReadFd(fd_m,saveError);
        if(len < 0){
            break;
        }
    }while(isET);
    return len;
}

ssize_t HttpConn::write(int *saveError){
    ssize_t len = -1;
    do{
        len = writev(fd_m,iov_m,iovCnt_m);
        if(len <= 0){
            *saveError = errno;
            break;
        }
        if(iov_m[0].iov_len + iov_m[1].iov_len == 0) {
            //传输结束
            break;
        }
        else if(static_cast<size_t>(len) > iov_m[0].iov_len){
            //iov_m[0]写满了，该写iov_m[1]了
            //iov_base是一个void指针，所以在对它进行偏移时要先定义好单位偏移量，即指针类型
            iov_m[1].iov_base = (char*)iov_m[1].iov_base + (len - iov_m[0].iov_len);
            iov_m[1].iov_len -= len - iov_m[0].iov_len;
            if(iov_m[0].iov_len){
                //清空iov_m[0]
                writeBuff_m.RetrieveAll();
                iov_m[0].iov_len = 0;
            }
        }
        else{
            //iov_m[0]还没有写满
            iov_m[0].iov_base = (char*)iov_m[0].iov_base + len;
            iov_m[0].iov_len -= len;
            writeBuff_m.Retrieve(len);
        }
    }while(isET || ToWriteBytes() > 10240);
    return len;
}

bool HttpConn::process(){
    request_m.Init();
    if(readBuff_m.ReadableBytes() <= 0){
        return false;
    }
    else if(request_m.parse(readBuff_m)){
        //TODO log
        response_m.Init(srcDir,request_m.path(),request_m.IsKeepAlive(),200);
    }
    else{
        response_m.Init(srcDir, request_m.path(),false, 400);
    }

    response_m.MakeResponse(writeBuff_m);

    //响应头
    iov_m[0].iov_base = const_cast<char*>(writeBuff_m.Peek());
    iov_m[0].iov_len = writeBuff_m.ReadableBytes();
    iovCnt_m = 1;

    //读取文件需要
    if(response_m.FileLen() > 0 && response_m.File()){
        iov_m[1].iov_base = response_m.File();
        iov_m[1].iov_len = response_m.FileLen();
        iovCnt_m = 2;
    }

    //TODO log

    return true;
}

void HttpConn::Close(){
    response_m.UnmapFile();
    if(isClose_m == false){
        isClose_m = true;
        userCount--;
        close(fd_m);
        //TODO 日志
    }
}
