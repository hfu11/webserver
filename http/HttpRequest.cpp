#include "HttpRequest.h"

using namespace std;

const unordered_set<string> HttpRequest::DEFAULT_HTML{
    "/index",
};

const unordered_map<string,int> HttpRequest::DEFAULT_HTML_TAG{

};

void HttpRequest::Init(){
    method_m = body_m = version_m = path_m = "";
    state_m = REQUEST_LINE;
    header_m.clear();
    post_m.clear();
}

bool HttpRequest::IsKeepAlive() const{
    if(header_m.count("Connection") == 1){
        return header_m.find("Connection")->second == "keep-alive" && version_m == "1.1";
    }
    return false;
}

bool HttpRequest::parse(Buffer& buff){
    const char CRLF[] = "\r\n";

    if(buff.)
}