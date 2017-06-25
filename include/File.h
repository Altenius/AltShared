#ifndef OPENCEMULATOR_FILE_H
#define OPENCEMULATOR_FILE_H

#include <cstdio>
#include <string>

class File
{
public:
    enum OpenMode
    {
        MODE_READ,
        MODE_WRITE,
        MODE_APPEND,
        MODE_READWRITE
    };

    File();

    /* Opens a file with the specified mode. Check for success
     * using isOpen(). */
    File(const std::string &path, OpenMode mode);

    ~File();

    /* Closes the file if it is open. */
    void close();

    /* Returns true if the file is open. */
    bool isOpen() const;

    /* Reads up to len bytes. Returns the amount of bytes
     * read or -1 on error. */
    int read(char *dst, size_t len);

    /* Writes up to len bytes. Returns the amount of bytes
     * written or -1 on error. */
    int write(const char *src, size_t len);

    /* Opens a file with the specified mode. Returns false if
     * the file could not be opened. */
    bool open(const std::string &path, OpenMode mode);

    /* Seeks to offset from whence. By default, whence
     * is the beginning of the file. Returns the new position
     * of the stream. */
    int seek(uint32_t offset, int whence = SEEK_SET);
    
    /* Returns the current position in the stream. */
    int tell();
    
    /* Reads the rest of the file into a string. Returns the amount
     * read. */
    int readRemaining(std::string &out);

private:
    FILE *m_file;
};

#endif //OPENCEMULATOR_FILE_H
