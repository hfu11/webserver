#include "HttpResponse.h"

using namespace std;

const unordered_map<string, string> HttpResponse::SUFFIX_TYPE = {
    {".html","text/html"},
    {".jpg","image/jpeg"},
    {".jpeg","image/jpeg"},
    {".png","image/png"},
    {".css","text/css"},
    {".js","text/javascript"},
};

const unordered_map<int, string> HttpResponse::CODE_STATUS ={
    {200,"OK"},
    {400,"Bad Request"},
    {403,"Forbidden"},
    {404,"Not Found"},
};

const unordered_map<int, string> HttpResponse::CODE_PATH ={
    {400, "/400.html"},
    {403, "/403.html"},
    {404, "/404.html"},
};

HttpResponse::HttpResponse(){
    code_m = -1;
    path_m = srcDir_m = "";
    isKeepAlive_m = false;
    mmFile_m = NULL;
    mmFileStat_m = {0};
}

HttpResponse::~HttpResponse(){
    UnmapFile();
}

void HttpResponse::Init(const string& srcDir, string& path, bool isKeepAlive, int code){
    assert(srcDir != "");
    if(mmFile_m) {UnmapFile();}
    code_m = code;
    isKeepAlive_m = isKeepAlive;
    path_m = path;
    srcDir_m = srcDir;
    mmFile_m = NULL;
    mmFileStat_m = {0};
}


void HttpResponse::MakeResponse(Buffer& buff){
    //判断请求的资源文件
    if(stat( (srcDir_m + path_m ).data() ,&mmFileStat_m) < 0 || S_ISDIR(mmFileStat_m.st_mode)){
        code_m = 404;
    }
    else if(!(mmFileStat_m.st_mode & S_IROTH)){
        //其他用户不可读
        code_m = 403;
    }
    else if(code_m == -1){
        code_m = 200;
    }

    ErrorHtml_m();
    AddStateLine_m(buff);
    AddHeader_m(buff);
    AddContent_m(buff);
}
void HttpResponse::ErrorHtml_m(){
    if(CODE_PATH.count(code_m) == 1){
        path_m = CODE_PATH.find(code_m)->second;
    }
}

void HttpResponse::AddStateLine_m(Buffer& buff){
    string status;
    if(CODE_STATUS.count(code_m) == 1){
        status = CODE_STATUS.find(code_m)->second;
    }
    else{
        //未定义行为
        code_m = 400;
        status = CODE_STATUS.find(400)->second;
    }
    buff.Append("HTTP/1.1 " + to_string(code_m) + " " + status + "\r\n");
}

void HttpResponse::AddHeader_m(Buffer& buff){
    buff.Append("Connection: ");
    if(isKeepAlive_m){
        buff.Append("keep-alive\r\n");
        buff.Append("keep-alive: max=6, timeout=120\r\n"); 
        //TODO定时器
    }
    else{
        buff.Append("close\r\n");
    }
    buff.Append("Content-type: " + GetFileType_m() + "\r\n");
}

void HttpResponse::AddContent_m(Buffer& buff){
    int srcFd = open((srcDir_m + path_m).data(),O_RDONLY);
    if(srcFd<0){
        ErrorContent(buff,"File NotFound!");
        return;
    }

    //内存映射
    //MAP_PRIVATE 建立一个写时拷贝的私有映射
    //TODO 日志模块
    int *mmRet = (int*)mmap(0,mmFileStat_m.st_size,PROT_READ,MAP_PRIVATE,srcFd,0);
    if(*mmRet == -1){
        ErrorContent(buff,"File NotFound!");
        return;
    }

    mmFile_m = (char*)mmRet;
    close(srcFd);
    buff.Append("Content-length: " + to_string(mmFileStat_m.st_size) + "\r\n\r\n");
}

void HttpResponse::UnmapFile(){
    if(mmFile_m){
        munmap(mmFile_m,mmFileStat_m.st_size);
        mmFile_m = NULL;
    }
}

string HttpResponse::GetFileType_m(){
    auto idx = path_m.find_last_of('.');
    if(idx == string::npos){
        return "text/plain";
    }
    string suffix = path_m.substr(idx);
    if(SUFFIX_TYPE.count(suffix) == 1){
        return SUFFIX_TYPE.find(suffix)->second;
    }
}

char* HttpResponse::File(){
    return mmFile_m;
}

void HttpResponse::ErrorContent(Buffer& buff, string msg){
    string body;
    string status;
    body += "<html><title>Error</title>";
    if(CODE_STATUS.count(code_m) == 1){
        status = CODE_STATUS.find(code_m)->second;
    }
    else{
        status = "Bad Request";
    }

    body += to_string(code_m) + " : " + status + "\n";
    body += "<p>" + msg + "</p>";

    buff.Append("Content-length: " + to_string(body.size()) + "\r\n\r\n");
    buff.Append(body);
}

size_t HttpResponse::FileLen() const{
    return mmFileStat_m.st_size;
}
