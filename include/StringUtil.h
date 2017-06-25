#ifndef VOCABBOT_UTIL_H
#define VOCABBOT_UTIL_H
#include <string>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <vector>

class StringUtil {
public:
    static inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if_not(s.begin(), s.end(),
                                            std::ptr_fun<int, int>(std::isspace)));
    }

    static inline void rtrim(std::string &s) {
        s.erase(std::find_if_not(s.rbegin(), s.rend(),
                                 std::ptr_fun<int, int>(std::isspace)).base(), s.end());
    }

    static inline void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }

    static inline std::string lower(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    }
    
    template <typename T>
    static bool stoi(const std::string &str, T &num, unsigned char base = 10) {
        char sign = 1;
        num = 0;
        auto it = str.begin();
        if (*it == '-') {
            it++;
            sign = -1;
        } else if (*it == '+') {
            it++;
        }
        for (; it != str.end(); it++) {
            unsigned char digit;
            if (*it >= 48 && *it <= 57) {
                digit = static_cast<unsigned char>(*it - '0');
            } else if (*it >= 65 && *it <= 70) { // max base of 16
                digit = static_cast<unsigned char>(*it - 'A');
            } else if (*it >= 97 && *it <= 102) {
                digit = static_cast<unsigned char>(*it - 'a');
            } else {
                return false;
            }
            num = num * base + digit;
        }
        
        num *= sign;
        return true;
    }
    
    static inline std::string absolutePath(const std::string &str) {
        std::vector<std::string> parts;
        std::string part;
        for (auto it = str.begin(); it <= str.end(); it++) {
            if (it == str.end() || *it == '/' || *it == '\\') {
                trim(part);
                if (!part.empty()) {
                    if (part != ".") {
                        if (part == "..") {
                            if (!parts.empty()) {
                                parts.pop_back();
                            }
                        } else {
                            parts.push_back(part);
                        }
                    }
                    part.clear();
                }
            } else {
                part += *it;
            }
        }
        
        if (parts.empty()) {
            return "/";
        }
        
        std::stringstream ns;
        for (std::string p : parts) {
            ns << "/" << p;
        }
        
        return ns.str();
    }
    
    static std::string percentEncode(const std::string &str) {
        std::stringstream encoded;
        for (auto it = str.begin(); it != str.end(); it++) {
            switch (*it) {
                case '!':
                case '#':
                case '$':
                case '&':
                case '\'':
                case '(':
                case ')':
                case '*':
                case '+':
                case ',':
                case '/':
                case ':':
                case ';':
                case '=':
                case '?':
                case '@':
                case '[':
                case ']':
                case '%':
                    encoded << "%" << std::hex << (int)(*it);
                    break;
                default:
                    encoded << *it;
            }
        }
        
        return encoded.str();
    }
    
    static bool percentDecode(const std::string &str, std::string &decoded) {
        unsigned char decoding = 0;
        char c = 0;
        for (auto it = str.begin(); it != str.end(); it++) {
            if (decoding != 0) {
                switch (*it) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        c = c * 16 + (*it - '0');
                        break;
                    case 'a':
                    case 'b':
                    case 'c':
                    case 'd':
                    case 'e':
                    case 'f':
                        c = c * 16 + (*it - 'a' + 10);
                        break;
                    case 'A':
                    case 'B':
                    case 'C':
                    case 'D':
                    case 'E':
                    case 'F':
                        c = c * 16 + (*it - 'A' + 10);
                        break;
                    default:
                        return false;
                }
                
                if (decoding == 2) {
                    decoding = 0;
                    decoded.push_back(c);
                    c = 0;
                } else {
                    decoding = 2;
                }
            } else if (*it == '%') {
                decoding = 1;
            } else {
                decoded.push_back(*it);
            }
        }
        
        return decoding == 0;
    }
};


#endif //VOCABBOT_UTIL_H
