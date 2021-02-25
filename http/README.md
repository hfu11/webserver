# HttpRequest 模块
HttpRequest用以解析HTTP请求
## 使用regex进行字符串匹配
c++ regex库的使用方法：
1.首先定义正则匹配的规则 
2.定义匹配结果的传出参数 
3.调用regex_match进行模式匹配 
```
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

```