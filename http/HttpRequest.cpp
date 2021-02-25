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

    if(buff.ReadableBytes() <= 0){
        return false;
    }

    //开始读数据
    while(buff.ReadableBytes() && state_m != FINISH){
        //search函数：给定两个迭代器区间（前两个参数决定），在区间内寻找字串（后两个参数决定），返回字串第一个出现的位置
        //找到这一行的结尾(停在CRLF前的iterator)
        const auto lineEnd = search(buff.Peek(),buff.BeginWriteConst(),CRLF,CRLF+2);
        string line(buff.Peek(),lineEnd);
        switch (state_m)
        {
            case REQUEST_LINE:{
                if(!ParseRequestLine_m(line)){
                    return false;
                }
                ParsePath_m();
                break;
            }
            case HEADERS:{
                ParseHeader_m(line);
                if(buff.ReadableBytes()<=2){
                    //说明没有BODY
                    state_m = FINISH;
                }
                break;
            }
            case BODY:{
                ParseBody_m(line);
                break;
            }
            default:
                break;
        }
        //读完了所有数据
        if(lineEnd == buff.BeginWrite()){break;}
        //取走buffer的数据。注意这里要+2(\r\n)，因为search返回的是找到\r\n前的iterator
        buff.RetrieveUntil(lineEnd+2);
    }

    //TODO 日志模块

    return true;
}

void HttpRequest::ParsePath_m(){
    if(path_m == "/"){
        path_m = "/index.html";
    }
    else{
        for(auto item:DEFAULT_HTML){
            if(item == path_m){
                path_m += ".html";
                break;
            }
        }
    }
}

bool HttpRequest::ParseRequestLine_m(const string& line){
    //[^ ]表示匹配除了空格之外的所有字符
    regex pattern("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    smatch sm;
    if(regex_match(line,sm,pattern)){
        method_m = sm[1];
        path_m = sm[2];
        version_m = sm[3];
        //状态转换
        state_m = HEADERS;
        return true;
    }

    //TODO 日志模块
    return false;
}

void HttpRequest::ParseHeader_m(const string& line){
    regex pattern("^([^:]*): ?(.*)$");
    smatch sm;
    if(regex_match(line,sm,pattern)){
        header_m[sm[1]] = sm[2];
    }
    else{
        state_m = BODY;
    }
}

void HttpRequest::ParseBody_m(const string& line){
    body_m = line;
    // TODO ParsePost_m();
    state_m = FINISH;
    //TODO 日志模块
}

int HttpRequest::ConverHex(char ch){
    if(ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    if(ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
}

string HttpRequest::path() const{
    return path_m;
}

string& HttpRequest::path(){
    return path_m;
}

string HttpRequest::method() const{
    return method_m;
}

string HttpRequest::version() const{
    return version_m;
}

#pragma region TODO

void HttpRequest::ParsePost_m(){
    //TODO
}

void HttpRequest::ParseFromURLencoded_m(){
    //TODO
}

bool HttpRequest::UserVerify(const std::string& name, const std::string& pwd, bool isLogin){
    //TODO
}

string HttpRequest::GetPost(const string& key) const{
    //TODO
}

string HttpRequest::GetPost(const char *key) const{
    //TODO
}
#pragma endregion

