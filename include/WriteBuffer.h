#ifndef ALTMANAGE_PACKETBUFFER_H
#define ALTMANAGE_PACKETBUFFER_H

#include <string>

class WriteBuffer
{
public:
    WriteBuffer();

    /* Writes a 32 bit integer encoded in little endian to the buffer */
    void writeLEInt(int32_t n);

    /* Writes an unsigned 32 bit integer encoded in little endian to the buffer */
    void writeLEUInt(uint32_t n);

    /* Writes a 16 bit integer encoded in little endian to the buffer */
    void writeLEInt16(int16_t n);

    /* Writes an unsigned 16 bit integer encoded in little endian to the buffer */
    void writeLEUInt16(uint16_t n);

    /* Writes a 64 bit integer encoded in little endian to the buffer */
    void writeLEInt64(int64_t n);

    /* Writes an unsigned 64 bit integer encoded in little endian to the buffer */
    void writeLEUInt64(uint64_t n);

    /* Writes an 8 bit integer encoded in little endian to the buffer */
    void writeLEInt8(int8_t n);

    /* Writes an unsigned 8 bit integer encoded in little endian to the buffer */
    void writeLEUInt8(uint8_t n);

    /* Writes data of size len to the buffer */
    void writeLEBuffer(const char *data, size_t len);


    /* Writes a 32 bit integer encoded in big endian to the buffer */
    void writeBEInt(int32_t n);

    /* Writes an unsigned 32 bit integer encoded in big endian to the buffer */
    void writeBEUInt(uint32_t n);

    /* Writes a 16 bit integer encoded in big endian to the buffer */
    void writeBEInt16(int16_t n);

    /* Writes an unsigned 16 bit integer encoded in big endian to the buffer */
    void writeBEUInt16(uint16_t n);

    /* Writes a 64 bit integer encoded in big endian to the buffer */
    void writeBEInt64(int64_t n);

    /* Writes an unsigned 64 bit integer encoded in big endian to the buffer */
    void writeBEUInt64(uint64_t n);

    /* Writes an 8 bit integer encoded in big endian to the buffer */
    void writeBEInt8(int8_t n);

    /* Writes an unsigned 8 bit integer encoded in big endian to the buffer */
    void writeBEUInt8(uint8_t n);

    /* Writes data of size len to the buffer */
    void writeBEBuffer(const char *data, size_t len);



    inline const char *data()
    {
        return buffer_.data();
    }



    inline size_t size()
    {
        return buffer_.size();
    }



private:
    std::string buffer_;
};


#endif //ALTMANAGE_PACKETBUFFER_H
