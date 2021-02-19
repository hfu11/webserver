#include "taskGET.h"
#include <iostream>
#include <event2/http.h>
#include <event2/bufferevent.h>
#include <string.h>
#include <evhttp.h>


using namespace std;

#define WEBROOT "."
#define DEFAULTINDEX "index.html"

void taskGET::Process(){
    cout<<"taskGET"<<endl;

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
    const char* uri = evhttp_request_get_uri(req);
    cout<<"uri: "<<uri<<endl;

    string filepath = WEBROOT;
    filepath += uri;
    if(strcmp(uri,"/")==0){
        filepath += DEFAULTINDEX;
    }

    //消息报头 要支持 图片、js、css、下载zip
    evkeyvalq *outhead = evhttp_request_get_output_headers(req);
    //获取文件的后缀名
    int pos = filepath.rfind('.');
    string postfix = filepath.substr(pos+1,filepath.size()-(pos+1));

    if(postfix == "jpg" || postfix == "gif" || postfix == "png"){
        //图片
        string tmp = "image/" + postfix;
        evhttp_add_header(outhead,"Content-Type",tmp.c_str());
    }
    else if(postfix == "zip"){
        string tmp = "application/" + postfix;
        evhttp_add_header(outhead,"Content-Type",tmp.c_str());
    }

    //读取html文件
    FILE* fp = fopen(filepath.c_str(),"r");
    if(!fp){
        evhttp_send_reply(req,HTTP_NOTFOUND,"",0);
        return;

    }

    evbuffer* outbuf = evhttp_request_get_output_buffer(req);
    //写入正文
    while(fread(buf,1,sizeof(buf),fp)){
        evbuffer_add(outbuf,buf,sizeof(buf));
    }
    
    evhttp_send_reply(req,HTTP_OK,"",outbuf);

}