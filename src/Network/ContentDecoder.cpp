#include <Network/ContentDecoder.h>
#include <cstring>
#include <stdint.h>
#include <zlib.h>

#define GZIP_BUFSIZE 8192

class GzipContentDecoder : public ContentDecoder
{
    friend ContentDecoder;

public:
    GzipContentDecoder(Callbacks &callbacks);

    virtual size_t parse(const char *data, size_t len) override;

    ~GzipContentDecoder();

private:
    z_stream stream;
    uint8_t buffer[GZIP_BUFSIZE];

    bool initialized_;
};


ContentDecoder::ContentDecoder(Callbacks &callbacks) : callbacks_(callbacks)
{
}


GzipContentDecoder::GzipContentDecoder(Callbacks &callbacks)
    : ContentDecoder(callbacks), initialized_(false)
{
    std::memset((void *)&stream, 0, sizeof(z_stream));

    int res = inflateInit2(&stream, 31); // 31: gzip only
    if (res != Z_OK)
    {
        callbacks_.onError("Could not initialize zlib stream");
        return;
    }

    initialized_ = true;
}


GzipContentDecoder::~GzipContentDecoder()
{
    if (initialized_)
    {
        inflateEnd(&stream);
    }
}


size_t GzipContentDecoder::parse(const char *data, size_t len)
{
    uint8_t *data_in = (uint8_t *)data;
    stream.next_in = data_in;
    stream.avail_in = len;
    stream.next_out = buffer;
    stream.avail_out = GZIP_BUFSIZE;


    do
    {
        int res = inflate(&stream, 0);
        if (!(res == Z_OK || res == Z_STREAM_END))
        {
            callbacks_.onError("inflate() failed");
            return 0;
        }

        if (stream.avail_out != GZIP_BUFSIZE)
        {
            callbacks_.onDecodedBodyData((char *)buffer,
                                         GZIP_BUFSIZE - stream.avail_out);
            stream.next_out = buffer;
            stream.avail_out = GZIP_BUFSIZE;
        }
    } while (stream.avail_out == 0);

    return len;
}


ContentDecoderPtr ContentDecoder::create(Callbacks &callbacks,
                                         const std::string &contentEncoding)
{
    if (contentEncoding == "gzip")
    {
        return ContentDecoderPtr(new GzipContentDecoder(callbacks));
    }
    else
    {
        return nullptr;
    }
}
