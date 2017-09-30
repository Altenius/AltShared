#ifndef OPENCEMULATOR_READBUFFER_H
#define OPENCEMULATOR_READBUFFER_H

#include <string>

class ReadBuffer
{
public:
    enum SeekDir
    {
        SK_BEG,
        SK_CUR,
        SK_END
    };

    virtual bool readBuffer(char *dst, size_t len) = 0;


    virtual void seek(uint32_t seekAmount, SeekDir dir) = 0;


    bool readString(std::string &string);


    template <typename T> bool readLE(T &n)
    {
        char buffer[sizeof(T)];
        if (!readBuffer(buffer, sizeof(T)))
        {
            return false;
        }

        n = 0;
        for (size_t i = 0; i < sizeof(T); ++i)
        {
            n |= (buffer[i] << (i * 8));
        }
        return true;
    }


    template <typename T> bool readBE(T &n)
    {
        char buffer[sizeof(T)];
        if (!readBuffer(buffer, sizeof(T)))
        {
            return false;
        }

        n = 0;
        for (size_t i = 0; i < sizeof(T); ++i)
        {
            n |= (buffer[sizeof(T) - i - 1] << (i * 8));
        }
        return true;
    }
};


#endif // OPENCEMULATOR_READBUFFER_H
