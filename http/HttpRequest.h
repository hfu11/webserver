#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include "../buffer/Buffer.h"
#include <regex>

class HttpRequest
{
public:

    enum PARSE_STATE{
        REQUEST_LINE,
        HEADERS,
        BODY,
        FINISH,
    };

    enum HTTP_CODE{
        NO_REQUEST = 0,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURCE,
        FORBIDDEN_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION,
    };

    HttpRequest() { Init();}
    ~HttpRequest() = default;

    void Init();
    bool parse(Buffer& buff);

    //const表示this指针为const指针，不能改变this所指向的值
    std::string path() const;
    std::string& path();
    std::string method() const;
    std::string version() const;
    std::string GetPost(const std::string& key) const;
    std::string GetPost(const char* key) const;

    bool IsKeepAlive() const;

private:
    bool ParseRequestLine_m(const std::string& line);
    void ParseHeader_m(const std::string& line);
    void ParseBody_m(const std::string& line);

    void ParsePath_m();
    void ParsePost_m();
    void ParseFromURLencoded_m();
    
    static bool UserVerify(const std::string& name, const std::string& pwd, bool isLogin);

    PARSE_STATE state_m;
    std::string  method_m, path_m, version_m, body_m;
    //头部可以解析为键值对形式
    std::unordered_map<std::string, std::string> header_m;
    //post请求可以解析为键值对形式
    std::unordered_map<std::string, std::string> post_m;

    static const std::unordered_set<std::string> DEFAULT_HTML;
    static const std::unordered_map<std::string,int> DEFAULT_HTML_TAG;
    
    static int ConverHex(char ch);


};


#endif