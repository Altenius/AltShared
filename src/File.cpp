#include "File.h"
#include <assert.h>


File::File() : m_file(nullptr)
{
}


File::~File()
{
    close();
}


File::File(const std::string &path, OpenMode mode) : m_file(nullptr)
{
    open(path, mode);
}


bool File::open(const std::string &path, OpenMode mode)
{
    assert(!isOpen());
    const char *oMode = nullptr;

    switch (mode)
    {
    case MODE_READ:
        oMode = "r";
        break;
    case MODE_APPEND:
        oMode = "a";
        break;
    case MODE_READWRITE:
        oMode = "rw";
        break;
    case MODE_WRITE:
        oMode = "w";
        break;
    case MODE_READ_BINARY:
        oMode = "rb";
        break;
    case MODE_WRITE_BINARY:
        oMode = "wb";
    }

    assert(oMode != nullptr);

    return ((m_file = fopen(path.c_str(), oMode)) != nullptr);
}


bool File::eof()
{
    assert(isOpen());

    return feof(m_file) != 0;
}


bool File::isOpen() const
{
    return m_file != nullptr;
}


void File::close()
{
    if (!isOpen())
    {
        return;
    }

    fclose(m_file);
    m_file = nullptr;
}


int File::read(char *dst, size_t len)
{
    assert(isOpen());

    return static_cast<int>(
        fread(reinterpret_cast<void *>(dst), 1, len, m_file));
}


int File::write(const char *src, size_t len)
{
    assert(isOpen());

    return static_cast<int>(
        fwrite(reinterpret_cast<const void *>(src), 1, len, m_file));
}


int File::seek(uint32_t offset, int whence)
{
    assert(isOpen());

    return fseek(m_file, offset, whence);
}


int File::tell()
{
    assert(isOpen());

    return static_cast<int>(ftell(m_file));
}


int File::readRemaining(std::string &out)
{
    assert(isOpen());

    int pos = tell();
    seek(0, SEEK_END);
    int amount = tell() - pos;
    seek(pos);

    char buffer[amount];

    size_t rd = fread(reinterpret_cast<void *>(buffer), 1, amount, m_file);
    out.assign(buffer, rd);

    return static_cast<int>(rd);
}
