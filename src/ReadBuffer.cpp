#include "ReadBuffer.h"



bool ReadBuffer::readString(std::string &string)
{
    uint32_t size;
    if (!readBE<uint32_t>(size)) {
        return false;
    }

    char data[size];

    if (!readBuffer(data, size)) {
        return false;
    }

    string.assign(data, size);
    return true;
}