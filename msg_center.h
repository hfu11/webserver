#ifndef MSGCENTER_H
#define MSGCENTER_H

#include "threadpool.h"
// #include "taskBase.h"

#define MAX_FD 65535
#define MAX_EVENT_NUMBER 10000

class taskBase;

class msg_center
{
public:
    static msg_center* Get(){
        static msg_center ins;
        return &ins;
    }
    // ~msg_center();

    void Dispatch(struct evhttp_request *req);
    
    void Init();

private:
    msg_center(/* args */);
    
    threadpool<taskBase> *pool =NULL;
};



#endif