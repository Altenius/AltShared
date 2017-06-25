#ifndef ALT_TRANSFERENCODINGPARSER_H
#define ALT_TRANSFERENCODINGPARSER_H

#include <string>
#include <memory>


class TransferEncodingParser;

typedef std::shared_ptr<TransferEncodingParser> TransferEncodingParserPtr;

class TransferEncodingParser
{
public:
    class Callbacks
    {
    public:
        virtual void onBodyData(const char *data, size_t len)
        {};



        virtual void onBodyComplete()
        {};



        virtual void onError(const std::string &error)
        {};
    };

    virtual size_t parse(const char *data, size_t len) =0;



    virtual void onClose()
    {};

    static TransferEncodingParserPtr
    create(Callbacks &callbacks, const std::string &transferEncoding, int contentLength);

protected:
    TransferEncodingParser(Callbacks &callbacks);

    Callbacks &callbacks_;

private:

};


#endif
