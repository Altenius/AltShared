#ifndef ALT_CONTENTDECODER_H
#define ALT_CONTENTDECODER_H

#include <memory>

class ContentDecoder;

typedef std::shared_ptr<ContentDecoder> ContentDecoderPtr;

class ContentDecoder
{
public:
    class Callbacks
    {
    public:
        virtual void onDecodedBodyData(const char *data, size_t len)
        {};



        virtual void onError(const std::string &error)
        {};
    };

    virtual size_t parse(const char *data, size_t len) =0;



    virtual void onClose()
    {};

    static ContentDecoderPtr create(Callbacks &callbacks, const std::string &contentEncoding);



    virtual ~ContentDecoder()
    {};
protected:
    ContentDecoder(Callbacks &callbacks);

    Callbacks &callbacks_;

private:

};

#endif