#ifndef LIBABYSS_CASCSTREAM_H
#define LIBABYSS_CASCSTREAM_H

#include <ios>
#include <streambuf>
#include <vector>
#include <libabyss/casc.h>

namespace LibAbyss {

    class CASCStream : public std::basic_streambuf<char> {
    public:
        CASCStream(HANDLE casc, std::string fileName);

        ~CASCStream() override;

    protected:
        int underflow() override;
        pos_type seekpos(pos_type pos,
                         std::ios_base::openmode which) override;
        pos_type seekoff(off_type off, std::ios_base::seekdir dir,
                         std::ios_base::openmode which) override;

    private:
        HANDLE _file = 0;
        std::streamsize _startOfBlock = 0;
        char _buffer[2048] = {};
    };

}

#endif //LIBABYSS_CASCSTREAM_H
