#ifndef ALT_COOKIE_H
#define ALT_COOKIE_H
#include <string>
#include <chrono>
#include <vector>
#include <memory>

class Cookie { 
public:
    typedef std::chrono::system_clock::time_point TimeType;
    Cookie(const std::string &name, const std::string &value);
    Cookie();
    
    static Cookie parse(const std::string &value, const std::string &domain, const std::string &path);
    
    inline bool valid() {
        return !name_.empty();
    }
    
    inline std::string name() {
        return name_;
    }
    
    inline std::string value() {
        return value_;
    }
    
    inline std::string domain() {
        return domain_;
    }
    
    inline std::string path() {
        return path_;
    }
    
    inline bool httpOnly() {
        return httpOnly_;
    }
    
    inline bool secure() {
        return secure_;
    }
    
    inline bool expired() {
        return expires_.time_since_epoch() != std::chrono::system_clock::duration::zero() && std::chrono::system_clock::now() > expires_;
    }
    
    inline bool expired(const std::chrono::system_clock::time_point &now) {
        return expires_.time_since_epoch() != std::chrono::system_clock::duration::zero() && now > expires_;
    }
    
    bool parseDate(const std::string &date);
    
    bool usable(const std::string &domain, const std::string &path); // path must be an absolute path (StringUtil::absolutePath())
    
protected:
    std::string name_, value_, domain_, path_;
    bool httpOnly_, secure_;
    TimeType expires_;

    static unsigned char parseMonth(const std::string &abvr);
};

class CookieJar;
typedef std::shared_ptr<CookieJar> CookieJarPtr;

class CookieJar {
public:
    
    void parse(const std::string &str, const std::string &domain, const std::string &path);
    
    std::string getCookies(const std::string &domain, const std::string &path);

    static inline CookieJarPtr create() {
        return CookieJarPtr(new CookieJar);
    }
protected:
    CookieJar();
    
private:
    std::vector<Cookie> cookies_;
};

#endif