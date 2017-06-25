#include "WriteBuffer.h"



void WriteBuffer::writeLEInt(int32_t n) {
    char data[4];
    data[0] = n & 0xFF;
    data[1] = (n >> 8) & 0xFF;
    data[2] = (n >> 16) & 0xFF;
    data[3] = n >> 24;
    
    buffer_.append(data, 4);
}

void WriteBuffer::writeLEUInt(uint32_t n) {
    char data[4];
    data[0] = n & 0xFF;
    data[1] = (n >> 8) & 0xFF;
    data[2] = (n >> 16) & 0xFF;
    data[3] = n >> 24;

    buffer_.append(data, 4);
}

