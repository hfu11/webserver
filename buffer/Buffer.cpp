#include "Buffer.h"

using namespace std;

Buffer::Buffer(int initBuffSize):buffer_m(initBuffSize),readPos_m(0),writePos_m(0){}

size_t Buffer::ReadableBytes() const {
    return writePos_m - readPos_m;
}

size_t Buffer::WritableBytes() const{
    return buffer_m.size() - writePos_m;
}

size_t Buffer::PrependableBytes() const{
    return readPos_m;
}

const char* Buffer::Peek() const{
    return BeginPtr_m() + readPos_m;
}

void Buffer::Retrieve(size_t len){
    assert(len <= ReadableBytes());
    readPos_m += len;
}

void Buffer::RetrieveUntil(const char* end){
    assert(Peek() <= end);
    Retrieve(end-Peek());
}

void Buffer::RetrieveAll(){
    buffer_m.clear();
    readPos_m = 0;
    writePos_m = 0;
}

string Buffer::RetrieveAllToStr(){
    string str(Peek(),ReadableBytes());
    RetrieveAll();
    return str;
}

const char* Buffer::BeginWriteConst() const{
    return BeginPtr_m() + writePos_m;
}

char* Buffer::BeginWrite(){
    return BeginPtr_m() + writePos_m;
}

void Buffer::HasWritten(size_t len){
    writePos_m += len;
}

//写入缓冲区
void Buffer::Append(const char* str,size_t len){
    assert(str);
    EnsureWritable(len);
    copy(str,str+len,BeginWrite());
    HasWritten(len);
}

void Buffer::Append(const Buffer& buff){
    Append(buff.Peek(),buff.ReadableBytes());
}

void Buffer::Append(const string& str){
    Append(str.data(),str.size());
}

void Buffer::Append(const void*data, size_t len){
    assert(data);
    Append(static_cast<const char*>(data),len);
}

void Buffer::EnsureWritable(size_t len){
    if(WritableBytes() < len){
        MakeSpace_m(len);
    }
    assert(WritableBytes() >= len);
}

ssize_t Buffer::ReadFd(int fd, int *saveErrno){
    struct iovec iov[2];
    const size_t writable = WritableBytes();

    char buff[65535];
    //分散读，确保读完所有数据。iov[0]读不下了再放在iov[1]里继续读
    iov[0].iov_base = BeginPtr_m() + writePos_m;    //先在原buffer里读
    iov[0].iov_len = writable;
    iov[1].iov_base = buff;     //读不下了放在buff[65535]里继续读
    iov[1].iov_len = sizeof(buff);

    const auto len = readv(fd,iov,2);
    if(len < 0){
        *saveErrno = errno;
    }
    else if(static_cast<size_t>(len) <= writable){
        HasWritten(len);
    }
    else{
        writePos_m = buffer_m.size();
        Append(buff,len-writable);
    }
    return len;
}

ssize_t Buffer::WriteFd(int fd, int *saveErrno){
    auto readable = ReadableBytes();
    auto len = write(fd,Peek(),readable);
    if(len < 0){
        *saveErrno = errno;
        return len;
    }
    readPos_m += len;
    return len;
}

char* Buffer::BeginPtr_m(){
    return &*buffer_m.begin();
}

const char* Buffer::BeginPtr_m() const{
    return &*buffer_m.begin();
}

void Buffer::MakeSpace_m(size_t len){
    if(WritableBytes() + PrependableBytes() < len){
        buffer_m.resize(writePos_m + len + 1);
    }
    else{
        auto readable = ReadableBytes();
        copy(BeginPtr_m(),BeginPtr_m()+writePos_m,BeginPtr_m());
        readPos_m = 0;
        writePos_m = readPos_m + readable;
        assert(readable == ReadableBytes());
    }
}

