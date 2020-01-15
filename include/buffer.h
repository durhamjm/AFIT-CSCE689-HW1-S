//Copied from https://stackoverflow.com/questions/11216125/c-read-ing-from-a-socket-to-an-ofstream

#include <memory>
#include <vector>


template <unsigned BUF_SIZE>
struct Buffer {
    char buffer[BUF_SIZE];
    int bufLen;
    Buffer () : buffer(), bufLen(0) {}
    int read (int fd) {
        int r = read(fd, buffer + bufLen, BUF_SIZE - bufLen);
        if (r > 0) bufLen += r;
        return r;
    }
    int capacity () const { return BUF_SIZE - bufLen; }
};

template <unsigned BUF_SIZE>
struct BufferStream {
    typedef std::unique_ptr< Buffer<BUF_SIZE> > BufferPtr;
    std::vector<BufferPtr> bufStream;
    BufferStream () : bufStream(1, BufferPtr(new Buffer<BUF_SIZE>)) {}
    int read (int fd) {
        if ((*bufStream.rbegin())->capacity() == 0)
            bufStream.push_back(BufferPtr(new Buffer<BUF_SIZE>));
        return (*bufStream.rbegin())->read(fd);
    }
};