#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <unistd.h>
#include <unordered_map>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <assert.h>

#include "../buffer/Buffer.h"

class HttpResponse
{
public:
    HttpResponse(/* args */);
    ~HttpResponse();

    void Init(const std::string& srcDir, std::string& path, bool isKeepAlive = false, int code = -1);
    void MakeResponse(Buffer& buff);
    void UnmapFile();
    char* File();
    size_t FileLen() const;
    void ErrorContent(Buffer& buff, std::string msg);
    int Code() const{ return code_m;}


    void AddStateLine_m(Buffer& buff);
    void AddHeader_m(Buffer &buff);
    void AddContent_m(Buffer &buff);

    void ErrorHtml_m();
    std::string GetFileType_m();

private:
    int code_m;
    bool isKeepAlive_m;
    std::string path_m;
    std::string srcDir_m;
    
    char* mmFile_m;
    struct stat mmFileStat_m;

    static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;
    static const std::unordered_map<int, std::string> CODE_STATUS;
    static const std::unordered_map<int, std::string> CODE_PATH;
};

#endif