#include "WriteBuffer.h"



WriteBuffer::WriteBuffer()
{

}



void WriteBuffer::writeString(const std::string &string)
{
    writeBE<uint32_t>(string.size());
    writeBuffer(string.data(), string.size());
}