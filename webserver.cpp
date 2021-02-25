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
#include <stdio.h>

#include "locker.h"
#include "threadpool.h"
#include "msg_center.h"

#define MAX_QUEUE_LENGTH 10000 
#define MAX_THREAD 16

using namespace std;



void http_cb(struct evhttp_request *req, void *arg){
    cout<<"con"<<endl;
    msg_center::Get()->Dispatch(req);
}

int main(){

    if(signal(SIGPIPE,SIG_IGN) == SIG_ERR ) return 1;

    msg_center::Get()->Init();

    auto base = event_base_new();

    //创建http上下文
    evhttp* evh = evhttp_new(base);

    //绑定端口
    if(evhttp_bind_socket(evh,"0.0.0.0",9999) != 0){
        cout<<"evhttp_bind_socket failed"<<endl;
    }

    //设定回调函数
    evhttp_set_gencb(evh,http_cb,NULL);

    if(base){
        //主循环
        event_base_dispatch(base);
    }

    if(base){
        event_base_free(base);
    }


    if(evh){
        evhttp_free(evh);
    }


    return 0;

}