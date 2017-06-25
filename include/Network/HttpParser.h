#ifndef ALT_HTTPPARSER_H
#define ALT_HTTPPARSER_H
#include <string>
#include <memory>
#include "HeaderParser.h"
#include "TransferEncodingParser.h"
#include "ContentDecoder.h"

class HttpParser : public HeaderParser::Callbacks, public TransferEncodingParser::Callbacks, public ContentDecoder::Callbacks {
public:
    class Callbacks {
    public:
        virtual void onStatusLine(const std::string &line) {};
        virtual void onHeadersFinished() {};
        virtual void onHeader(const std::string &key, const std::string &value) {};
        virtual void onBody(const std::string &body) {};
        virtual void onError(const std::string &error) {};
    };
    typedef std::shared_ptr<Callbacks> CallbacksPtr;
    
    HttpParser(Callbacks *callbacks);
    
    size_t parse(const char *data, size_t size);
    
    virtual void onHeader(const std::string &key, const std::string &value) override;
    virtual void onBodyData(const char *data, size_t len) override;
    virtual void onBodyComplete() override;
    virtual void onError(const std::string &error) override;
    virtual void onDecodedBodyData(const char *data, size_t len) override;
    
    void onRemoteClose();
    void reset();
private:
    std::string statusLine_;
    std::string buffer_;
    std::string bodyBuffer_;
    
    std::string transferEncoding_, contentEncoding_;
    long contentLength_;
    
    Callbacks *callbacks_;
    HeaderParser headerParser_;
    TransferEncodingParserPtr transferEncodingParser_;
    ContentDecoderPtr contentDecoder_;

    size_t parseStatusLine();
    size_t parseBody(const char *data, size_t size);
    void headersFinished();
};


#endif //VOCABBOT_HTTPPARSER_H
