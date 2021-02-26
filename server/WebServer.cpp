#include "WebServer.h"

WebServer::WebServer(
    int port, int trigMode, int timeoutMS, int OptLinger,
    //sql
    int threadNum
    //log
){
    srcDir_m = getcwd(NULL,256);
    assert(srcDir_m);
    
}