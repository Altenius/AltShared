#include "ReadStringBuffer.h"
#include <cstring>



bool ReadStringBuffer::readBuffer(char *dst, size_t len)
{
    if (buffer_ == nullptr || (bufferEnd_ - pointer_ < len)) {
        return false;
    }

    memcpy(dst, pointer_, len);
    pointer_ += len;

    return true;
}



ReadStringBuffer::~ReadStringBuffer()
{
    free(buffer_);
}



ReadStringBuffer::ReadStringBuffer(const std::string &string)
{
    if ((buffer_ = reinterpret_cast<char *>(malloc(string.length()))) == nullptr) {
        return;
    }
    pointer_ = buffer_;
    bufferEnd_ = buffer_ + string.length();

    std::memcpy(buffer_, string.data(), string.length());
}



ReadStringBuffer::ReadStringBuffer(const char *data, size_t len)
{
    if ((buffer_ = reinterpret_cast<char *>(malloc(len))) == nullptr) {
        return;
    }
    pointer_ = buffer_;
    bufferEnd_ = buffer_ + len;

    std::memcpy(buffer_, data, len);
}



void ReadStringBuffer::seek(uint32_t seekAmount, SeekDir dir)
{
    switch (dir) {
        case SK_BEG:
            pointer_ = buffer_ + seekAmount;
            break;
        case SK_CUR:
            pointer_ += seekAmount;
            break;
        case SK_END:
            pointer_ = bufferEnd_ + seekAmount;
            break;
    }
    
    if (pointer_ < buffer_) {
        pointer_ = buffer_;
    }
}