#ifndef TASKBASE_H
#define TASKBASE_H

class taskBase
{
public:
    struct evhttp_request* req = 0;
    // int sock = 0;
    int thread_id = 0;
    
    virtual void Process() = 0;

};

#endif