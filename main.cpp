#include <unistd.h>
#include "server/WebServer.h"

using namespace std;

int main(){

    WebServer server(
        9999,3,60000,false,
        //sql
        8
        //log
    );

    // server.Start();

    return 0;

}