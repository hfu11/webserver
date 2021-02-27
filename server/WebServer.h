#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <arpa/inet.h>
#include <unordered_map>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <functional>

#include "Epoller.h"
#include "../http/HttpConn.h"
#include "../pool/threadpool.h"


class WebServer
{
private:
    /* data */
public:
    WebServer(
        int port, int trigMode, int timeoutMS, int OptLinger,
        //sql
        int threadNum
        //log
    );
    ~WebServer();

    void Start();

    bool InitSocket_m();
    void InitEventMode_m(int trigMode);
    void AddClient_m(int fd, sockaddr_in addr);

    void DealListen_m();
    void DealWrite_m(HttpConn* client);
    void DealRead_m(HttpConn* client);

    void SendError_m(int fd, const char* info);
    void ExtentTime_m(HttpConn* clinet);
    void CloseConn_m(HttpConn* client);

    void OnRead_m(HttpConn* client);
    void OnWrite_m(HttpConn* client);
    void OnProcess(HttpConn* client);

    static const int MAX_FD = 65535;

    static int SetFdNonblock(int fd);

    int port_m;
    bool openLinger_m;
    int timeoutMS_m;
    bool isClose_m;
    int listenFd_m;
    char* srcDir_m;

    uint32_t listenEvent_m;
    uint32_t connEvent_m;

    //TODO Timer std::unique_ptr<HeapTimer>
    std::unique_ptr<threadpool<HttpConn>> threadpool_m;
    std::unique_ptr<Epoller> epoller_m;
    std::unordered_map<int, HttpConn> users_m;
};

#endif