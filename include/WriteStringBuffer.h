#ifndef OPENCEMULATOR_WRITESTRINGBUFFER_H
#define OPENCEMULATOR_WRITESTRINGBUFFER_H

#include "WriteBuffer.h"

class WriteStringBuffer : public WriteBuffer
{
public:
    void writeBuffer(const char *data, size_t len)
    {
        buffer_.insert(buffer_.end(), data, data + len);
    }


    inline const std::string &buffer()
    {
        return buffer_;
    }


private:
    std::string buffer_;
};

#endif // OPENCEMULATOR_WRITESTRINGBUFFER_H
