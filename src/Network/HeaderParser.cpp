#include "Network/HeaderParser.h"
#include "StringUtil.h"
#include <algorithm>


HeaderParser::HeaderParser(Callbacks *callbacks)
    : callbacks_(callbacks), done_(false)
{
}


size_t HeaderParser::parse(const char *data, size_t len)
{
    if (done_)
    {
        return 0;
    }

    size_t sStart = buffer_.size();
    sStart = (sStart > 1) ? sStart - 1 : 0; // start searching where we left off
    buffer_.append(data, len);

    size_t lineEnd = buffer_.find("\r\n", sStart);
    if (lineEnd == std::string::npos)
    {
        // not a full line
        return len;
    }

    size_t last = 0;
    do
    {
        if (last == lineEnd)
        {
            done_ = true;
            size_t bLen = buffer_.size();
            buffer_.clear();
            sendLast();
            return len - (bLen - lineEnd) + 2;
        }

        if (!parseLine(data + last, lineEnd - last))
        {
            // error
            done_ = true;
            return std::string::npos;
        }

        last = lineEnd + 2;
        lineEnd = buffer_.find("\r\n", last);
    } while (lineEnd != std::string::npos);

    buffer_.erase(0, last);
    return len;
}


bool HeaderParser::parseLine(const char *data, size_t len)
{
    if (data[0] == ' ' || data[0] == '\t')
    { // continuation
        if (lastKey_.empty())
        {
            return false;
        }

        lastValue_.append(data + 1, len);
        return true;
    }

    sendLast();
    auto pos = std::find(data, data + len, ':');
    if (pos == data + len || pos == data)
    { // no colon or blank key
        return false;
    }

    lastKey_.assign(data, pos);
    if (pos != data + len - 1)
    {
        lastValue_.assign(pos + 1, data + len);
    }
    else
    {
        lastValue_.clear();
    }
    return true;
}


void HeaderParser::sendLast()
{
    if (!lastKey_.empty())
    {
        StringUtil::trim(lastKey_);
        StringUtil::trim(lastValue_);
        callbacks_->onHeader(lastKey_, lastValue_);
        lastKey_.clear();
        lastValue_.clear();
    }
}


void HeaderParser::reset()
{
    lastKey_.clear();
    lastValue_.clear();
    done_ = false;
    buffer_.clear();
}
