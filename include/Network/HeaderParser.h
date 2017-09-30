#ifndef ALT_HEADERPARSER_H
#define ALT_HEADERPARSER_H

#include <memory>

class HeaderParser
{
public:
    class Callbacks
    {
    public:
        virtual void onHeader(const std::string &key,
                              const std::string &value){};
    };

    typedef std::shared_ptr<Callbacks> CallbacksPtr;

    HeaderParser(Callbacks *callbacks);

    size_t parse(const char *data, size_t len);


    inline bool done()
    {
        return done_;
    }


    void reset();

private:
    bool done_;
    Callbacks *callbacks_;
    std::string buffer_;
    std::string lastKey_, lastValue_;

    bool parseLine(const char *data, size_t len);

    void sendLast();
};


#endif
