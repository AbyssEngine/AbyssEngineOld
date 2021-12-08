#ifndef LIBABYSS_MPQSTREAM_H
#define LIBABYSS_MPQSTREAM_H

#include <ios>
#include <streambuf>
#include <vector>
#include <libabyss/mpq.h>
#include <libabyss/inputstream.h>

namespace LibAbyss {

    class MPQStream : public SizeableStreambuf {
    public:
        MPQStream(void* mpq, const std::string& fileName);

        ~MPQStream() override;

        std::streamsize StartOfBlockForTesting() const {
            return _startOfBlock;
        }

    protected:
        int underflow() override;
        pos_type seekpos(pos_type pos,
                         std::ios_base::openmode which) override;
        pos_type seekoff(off_type off, std::ios_base::seekdir dir,
                         std::ios_base::openmode which) override;
        std::streamsize size() const override;

    private:
        void* _mpqFile = 0;
        std::streamsize _startOfBlock = 0;
        char _buffer[2048] = {};
    };

}

#endif //LIBABYSS_MPQSTREAM_H
