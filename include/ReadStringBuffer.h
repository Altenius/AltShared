#ifndef OPENCEMULATOR_READSTRINGBUFFER_H
#define OPENCEMULATOR_READSTRINGBUFFER_H

#include "ReadBuffer.h"
#include <string>

class ReadStringBuffer : public ReadBuffer
{
public:
    ReadStringBuffer(const std::string &string);

    ReadStringBuffer(const char *data, size_t len);

    ~ReadStringBuffer();


    virtual bool readBuffer(char *dst, size_t len) override;

    virtual void seek(uint32_t seekAmount, SeekDir dir) override;

private:
    char *buffer_;
    char *bufferEnd_;
    char *pointer_;
};

#endif // OPENCEMULATOR_READSTRINGBUFFER_H
