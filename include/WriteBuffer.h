#ifndef ALTMANAGE_PACKETBUFFER_H
#define ALTMANAGE_PACKETBUFFER_H

#include <string>
#include <vector>

class WriteBuffer
{
public:
    WriteBuffer();


    /* Writes data of size len to the buffer */
    virtual void writeBuffer(const char *data, size_t len) = 0;


    void writeVector(const std::vector<char> &b)
    {
        writeBuffer(b.data(), b.size());
    }


    /* Writes a string to the buffer. Strings are written by
     * writing a uint32_t of the size of the string in big endian
     * andthen writing the char array */
    void writeString(const std::string &string);


    template <typename T> void writeLE(const T &n)
    {
        char data[sizeof(T)];
        for (size_t i = 0; i < sizeof(T); ++i)
        {
            data[i] = (n >> (8 * i)) & 0xFF;
        }
        writeBuffer(data, sizeof(T));
    }


    template <typename T> void writeBE(const T &n)
    {
        char data[sizeof(T)];
        for (size_t i = 0; i < sizeof(T); ++i)
        {
            data[sizeof(n) - i - 1] = (n >> (8 * i)) & 0xFF;
        }
        writeBuffer(data, sizeof(T));
    }
};


#endif // ALTMANAGE_PACKETBUFFER_H
