#include <event2/event.h>
#include <event2/bufferevent.h>
#include <iostream>
#include <signal.h>
#include <event2/listener.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <event2/buffer.h>
#include <evhttp.h>
#include <event2/http.h>

#include "msg_center.h"
#include "taskGET.h"
#include "taskPOST.h"

using namespace std;

void msg_center::Dispatch(evhttp_request *req){
    //获取请求类型并分发
    switch(evhttp_request_get_command(req)){
        case EVHTTP_REQ_GET:{
            taskBase *t = new taskGET();
            t->req = req;
            pool->append(t);
            break;
        }
        case EVHTTP_REQ_POST:{
            //POST
            taskBase *t = new taskPOST();
            t->req = req;
            pool->append(t);
            break;
        }
        default:{
            break;
        }
    }
}

void msg_center::Init(){
    pool = new threadpool<taskBase>;
}


msg_center::msg_center(/* args */)
{
}