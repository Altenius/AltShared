#include "Network/TransferEncodingParser.h"
#include "Network/HeaderParser.h"
#include "StringUtil.h"
#include <algorithm>


class IdentityEncodingParser : public TransferEncodingParser
{
    friend TransferEncodingParser;

public:
    virtual size_t parse(const char *data, size_t len) override
    {
        if (contentLength_ == 0 || len == 0)
        {
            return 0;
        }
        len = std::min(contentLength_, len);
        contentLength_ -= len;

        callbacks_.onBodyData(data, len);
        if (contentLength_ == 0)
        {
            callbacks_.onBodyComplete();
        }

        return len;
    }


protected:
    IdentityEncodingParser(Callbacks &callbacks, size_t contentLength)
        : contentLength_(contentLength), TransferEncodingParser(callbacks)
    {
    }


private:
    size_t contentLength_;
};


class CloseEncodingParser : public TransferEncodingParser
{
    friend TransferEncodingParser;

public:
    virtual size_t parse(const char *data, size_t len) override
    {
        if (len != 0)
        {
            callbacks_.onBodyData(data, len);
        }

        return len;
    }


    virtual void onClose() override
    {
        callbacks_.onBodyComplete();
    }


protected:
    CloseEncodingParser(Callbacks &callbacks)
        : TransferEncodingParser(callbacks)
    {
    }
};


class ChunkedEncodingParser : public TransferEncodingParser,
                              public HeaderParser::Callbacks
{
    friend TransferEncodingParser;

public:
    virtual size_t parse(const char *data, size_t len) override;

    virtual void onHeader(const std::string &key,
                          const std::string &value) override;

protected:
    ChunkedEncodingParser(TransferEncodingParser::Callbacks &callbacks);

private:
    enum Mode
    {
        PARSING_CHUNK_LENGTH,
        PARSING_CHUNK_EXT,
        PARSING_CHUNK_HDR_LF,
        PARSING_CHUNK_DATA,
        PARSING_CHUNK_DATA_CR,
        PARSING_CHUNK_DATA_LF,
        PARSING_TRAILER,
        PARSING_FINISHED
    };

    Mode mode_;
    size_t chunkSize_;

    inline size_t parseChunkLength(const char *data, size_t len);

    inline size_t parseChunkExt(const char *data, size_t len);

    inline size_t parseChunkHdrLf(const char *data, size_t len);

    inline size_t parseChunkData(const char *data, size_t len);

    inline size_t parseChunkDataCr(const char *data, size_t len);

    inline size_t parseChunkDataLf(const char *data, size_t len);

    inline size_t parseTrailer(const char *data, size_t len);

    HeaderParser trailerParser_;
};


TransferEncodingParser::TransferEncodingParser(
    TransferEncodingParser::Callbacks &callbacks)
    : callbacks_(callbacks)
{
}


TransferEncodingParserPtr
TransferEncodingParser::create(TransferEncodingParser::Callbacks &callbacks,
                               const std::string &transferEncoding,
                               int contentLength)
{

    if ((transferEncoding.empty() || transferEncoding == "identity") &&
        contentLength != -1)
    {
        return TransferEncodingParserPtr(
            new IdentityEncodingParser(callbacks, contentLength));
    }
    else if (transferEncoding == "chunked")
    {
        return TransferEncodingParserPtr(new ChunkedEncodingParser(callbacks));
    }
    else if (transferEncoding.empty())
    {
        return TransferEncodingParserPtr(new CloseEncodingParser(callbacks));
    }

    return nullptr;
}


ChunkedEncodingParser::ChunkedEncodingParser(
    TransferEncodingParser::Callbacks &callbacks)
    : mode_(PARSING_CHUNK_LENGTH), TransferEncodingParser(callbacks),
      trailerParser_(this), chunkSize_(0)
{
}


size_t ChunkedEncodingParser::parse(const char *data, size_t len)
{
    size_t oLen = len;
    do
    {
        size_t consumed = 0;
        switch (mode_)
        {
        case PARSING_CHUNK_LENGTH:
            consumed = parseChunkLength(data, len);
            break;
        case PARSING_CHUNK_EXT:
            consumed = parseChunkExt(data, len);
            break;
        case PARSING_CHUNK_HDR_LF:
            consumed = parseChunkHdrLf(data, len);
            break;
        case PARSING_CHUNK_DATA:
            consumed = parseChunkData(data, len);
            break;
        case PARSING_CHUNK_DATA_CR:
            consumed = parseChunkDataCr(data, len);
            break;
        case PARSING_CHUNK_DATA_LF:
            consumed = parseChunkDataLf(data, len);
            break;
        case PARSING_TRAILER:
            consumed = parseTrailer(data, len);
            break;
        }

        if (consumed == std::string::npos)
        {
            return std::string::npos;
        }

        data += consumed;
        len -= consumed;
    } while (len > 0);

    return oLen - len;
}


size_t ChunkedEncodingParser::parseChunkLength(const char *data, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        switch (data[i])
        {
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
            chunkSize_ = chunkSize_ * 16 + static_cast<size_t>(data[i] - '0');
            break;
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            chunkSize_ =
                chunkSize_ * 16 + static_cast<size_t>(data[i] - 'a' + 10);
            break;
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            chunkSize_ =
                chunkSize_ * 16 + static_cast<size_t>(data[i] - 'A' + 10);
            break;
        case ';':
            mode_ = PARSING_CHUNK_EXT;
            return i + 1;
        case '\r':
            mode_ = PARSING_CHUNK_HDR_LF;
            return i + 1;
        default:
            callbacks_.onError("invalid character in chunk length");
            return std::string::npos;
        }
    }

    return len;
}


size_t ChunkedEncodingParser::parseChunkExt(const char *data, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        switch (data[i])
        {
        case '\r':
            mode_ = PARSING_CHUNK_HDR_LF;
            return i + 1;
        default:
            break;
        }
    }

    return len;
}


size_t ChunkedEncodingParser::parseChunkHdrLf(const char *data, size_t len)
{
    if (len == 0)
    {
        return 0;
    }

    if (data[0] == '\n')
    {
        if (chunkSize_ == 0)
        {
            mode_ = PARSING_TRAILER;
        }
        else
        {
            mode_ = PARSING_CHUNK_DATA;
        }

        return 1;
    }

    callbacks_.onError("expected \\n after chunk header");
    return std::string::npos;
}


size_t ChunkedEncodingParser::parseChunkData(const char *data, size_t len)
{
    len = std::min(len, chunkSize_);

    callbacks_.onBodyData(data, len);
    chunkSize_ -= len;
    if (chunkSize_ == 0)
    {
        mode_ = PARSING_CHUNK_DATA_CR;
    }
    return len;
}


size_t ChunkedEncodingParser::parseChunkDataCr(const char *data, size_t len)
{
    if (len == 0)
    {
        return 0;
    }

    if (data[0] == '\r')
    {
        mode_ = PARSING_CHUNK_DATA_LF;
        return 1;
    }

    callbacks_.onError("expected \\r after chunk data");
    return std::string::npos;
}


size_t ChunkedEncodingParser::parseChunkDataLf(const char *data, size_t len)
{
    if (len == 0)
    {
        return 0;
    }

    if (data[0] == '\n')
    {
        mode_ = PARSING_CHUNK_LENGTH;
        return 1;
    }

    callbacks_.onError("expected \\r\\n after chunk data");
    return std::string::npos;
}


size_t ChunkedEncodingParser::parseTrailer(const char *data, size_t len)
{
    size_t ret = trailerParser_.parse(data, len);
    if (ret == std::string::npos)
    {
        callbacks_.onError("failed to parse trailer");
    }
    if (trailerParser_.done())
    {
        callbacks_.onBodyComplete();
        mode_ = PARSING_FINISHED;
    }
    return ret;
}


void ChunkedEncodingParser::onHeader(const std::string &key,
                                     const std::string &value)
{
}
