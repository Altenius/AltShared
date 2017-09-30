#ifndef _BASE64_H_
#define _BASE64_H_

// Stolen from
// https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c

#include <string>
#include <vector>
typedef unsigned char BYTE;

class Base64
{
public:
    static std::string encode(const std::vector<BYTE> &buf);
    static std::string encode(const BYTE *buf, unsigned int bufLen);
    static std::vector<BYTE> decode(std::string encoded_string);
};

#endif
