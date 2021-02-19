#include "taskPOST.h"
#include <iostream>
#include <event2/http.h>
#include <event2/bufferevent.h>
#include <string.h>
#include <evhttp.h>


#define WEBROOT "."
#define DEFAULTINDEX "index.html"

using namespace std;

void taskPOST::Process(){
    cout<<"taskPOST"<<endl;

    //消息报头
    evkeyvalq* headers = evhttp_request_get_input_headers(req);
    cout<<"======headers======\n"<<endl;
    for(evkeyval *p = headers->tqh_first;p;p=p->next.tqe_next){
        cout<<p->key<<" : "<<p->value<<endl;
    }

    //请求正文 (GET为空，POST有表单信息)
    evbuffer* inbuf = evhttp_request_get_input_buffer(req);
    char buf[1024] = {0};
    cout<<"=======Input Data======="<<endl;
    while(evbuffer_get_length(inbuf)){
        int len = evbuffer_remove(inbuf,buf,sizeof(buf)-1);
        if(len > 0){
            buf[len] = '\0';
            cout<<buf<<endl;
        }
    }

    //回复浏览器
    //状态行 消息报头 响应正文

    //分析出请求的文件 uri
    //设置根目录

    //uri
    // const char* uri = evhttp_request_get_uri(req);
    // cout<<"uri: "<<uri<<endl;

    // string filepath = WEBROOT;
    // filepath += uri;
    // if(strcmp(uri,"/")==0){
    //     filepath += DEFAULTINDEX;
    // }

    //输出消息报头
    // evkeyvalq *outhead = evhttp_request_get_output_headers(req);


}