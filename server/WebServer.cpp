#include "WebServer.h"

using namespace std;

WebServer::WebServer(
    int port, int trigMode, int timeoutMS, int OptLinger,
    //sql
    int threadNum
    //log
){
    srcDir_m = getcwd(NULL,256);
    assert(srcDir_m);
    // cout<<srcDir_m<<endl;
    strncat(srcDir_m,"/resources/",16);

    HttpConn::userCount = 0;
    HttpConn::srcDir = srcDir_m;
    //TODO 连接数据库

    InitEventMode_m(trigMode);
    if(!InitSocket_m()){isClose_m = true;}

    //TODO 日志
    
}

WebServer::~WebServer(){
    close(listenFd_m);
    isClose_m = true;
    free(srcDir_m);
    //TODO关闭数据库连接
}

void WebServer::Start(){
    int timeMS = -1;    //epoll wait timeout = -1 epoll_wait没有事件将一直阻塞
    //TODO log
    while(!isClose_m){
        if(timeoutMS_m >0){
            //TODO 设置计时器
        }
        int eventCnt = epoller_m->Wait(timeMS);
        for(int i = 0;i < eventCnt; i++){
            //处理事件
            int fd = epoller_m->GetEventFd(i);
            auto events = epoller_m->GetEvents(i);
            if(fd == listenFd_m){
                DealListen_m();
            }
            else if(events & (EPOLLRDHUP |  EPOLLHUP | EPOLLERR)){
                assert(users_m.count(fd) > 0);
                CloseConn_m(&users_m[fd]);
            }
            else if(events & EPOLLIN){
                assert(users_m.count(fd) > 0);
                DealRead_m(&users_m[fd]);
            }
            else if()
        }
    }
}

bool WebServer::InitSocket_m(){
    int ret;
    sockaddr_in addr;
    if(port_m > 65535 || port_m < 1024){
        //TODO log
        return false;
    }

    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_m);
    linger optLinger = {0};
    if(openLinger_m){
        //优雅关闭，知道所剩数据发送完毕或超时
        optLinger.l_onoff = 1;
        optLinger.l_linger = 1;
    }

    listenFd_m = socket(AF_INET,SOCK_STREAM,0);
    if(listenFd_m){
        //log
        return false;
    }

    ret = setsockopt(listenFd_m,SOL_SOCKET,SO_LINGER,&optLinger, sizeof(optLinger));
    if(ret < 0){
        close(listenFd_m);
        //log
        return false;
    }

    int optval = 1;
    ret = setsockopt(listenFd_m,SOL_SOCKET,SO_REUSEADDR, (const void*)&optval, sizeof(int));
    if(ret == -1){
        //log
        close(listenFd_m);
        return false;
    }

    ret = bind(listenFd_m,(sockaddr*)&addr,sizeof(addr));
    if(ret < 0){
        //log
        close(listenFd_m);
        return false;
    }

    ret = listen(listenFd_m,5);
    if(ret < 0){
        //log
        close(listenFd_m);
        return false;
    }

    ret = epoller_m->AddFd(listenFd_m,listenEvent_m | EPOLLIN);
    if(ret == 0){
        //log
        close(listenFd_m);
        return false;
    }

    SetFdNonblock(listenFd_m);

    //log

    return true;

}

void WebServer::InitEventMode_m(int trigMode){
    //listen监听读关闭事件
    listenEvent_m = EPOLLRDHUP;
    //
    connEvent_m = EPOLLONESHOT || EPOLLRDHUP;

    switch (trigMode)
    {
        case 0:{
            break;
        }
        case 1:{
            connEvent_m |= EPOLLET;
            break;
        }
        case 2:{
            listenEvent_m |= EPOLLET;
            break;
        }
        case 3:{
            listenEvent_m |= EPOLLET;
            connEvent_m |= EPOLLET;
            break;
        }
        default:{
            listenEvent_m |= EPOLLET;
            connEvent_m |= EPOLLET;
            break;
        }
    }
    HttpConn::isET = (connEvent_m & EPOLLET);
}

void WebServer::AddClient_m(int fd, sockaddr_in addr){
    assert(fd > 0);
    users_m[fd].init(fd,addr);
    if(timeoutMS_m > 0){
        //TODO 设置定时器
    }
    epoller_m->AddFd(fd,EPOLLIN | connEvent_m);
    SetFdNonblock(fd);
    //TODO log
}

void WebServer::DealListen_m(){
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    do{
        int fd = accept(listenFd_m,(sockaddr*)&addr,&len);
        if(fd <= 0){return;}
        else if(HttpConn::userCount >= MAX_FD){
            SendError_m(fd,"Server Busy!");
            //TODO log
            return;
        }
        AddClient_m(fd,addr);
    }while(listenEvent_m & EPOLLET);
}

void WebServer::DealWrite_m(HttpConn* client){
    assert(client);
    ExtentTime_m(client);
    threadpool_m->append(client);
}

void WebServer::DealRead_m(HttpConn* client){
    assert(client);
    ExtentTime_m(client);
    threadpool_m->append(client);
}

void WebServer::SendError_m(int fd, const char* info){

}

void WebServer::ExtentTime_m(HttpConn* client){
    assert(client);
    if(timeoutMS_m > 0){
        //定时器
    }
}

void WebServer::CloseConn_m(HttpConn* client){

}

void WebServer::OnRead_m(HttpConn* client){
    assert(client);
    int ret = -1;
    int readErrno = 0;
    ret = client->read(&readErrno);
    if(ret <= 0 &&readErrno != EAGAIN){
        CloseConn_m(client);
        return;
    }
    OnProcess(client);
}

void WebServer::OnWrite_m(HttpConn* client){

}

void WebServer::OnProcess(HttpConn* client){
    if(client->process()){
        epoller_m->ModFd(client->GetFd(),connEvent_m | EPOLLOUT);
    }
    else{
        epoller_m->ModFd(client->GetFd(),connEvent_m | EPOLLIN);
    }
}

int WebServer::SetFdNonblock(int fd){
    assert(fd > 0);
    auto flags = fcntl(fd,F_GETFL);
    flags |= O_NONBLOCK;

    return fcntl(fd, F_SETFL, flags);
}