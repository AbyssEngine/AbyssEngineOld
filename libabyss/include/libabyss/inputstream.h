#ifndef LIBABYSS_INPUTSTREAM_H
#define LIBABYSS_INPUTSTREAM_H

#include <ios>
#include <istream>
#include <memory>
#include <streambuf>

namespace LibAbyss {

class SizeableStreambuf : public std::basic_streambuf<char> {
    public:
        virtual std::streamsize size() const = 0;
};

class InputStream : public std::istream {
  public:
    explicit InputStream(std::unique_ptr<std::streambuf> streamBuff) : std::istream(streamBuff.get()), _streamBuff(std::move(streamBuff)) {}

    InputStream(InputStream &&other) noexcept : std::istream(other._streamBuff.get()), _streamBuff(std::move(other._streamBuff)) {}

    std::streamsize size() {
        if (auto* sizeable = dynamic_cast<SizeableStreambuf*>(_streamBuff.get())) {
            return sizeable->size();
        }
        const auto curPos = tellg();
        seekg(0, std::ios_base::end);
        const auto endPos = tellg();
        seekg(curPos, std::ios_base::beg);
        return endPos;
    }

  private:
    std::unique_ptr<std::streambuf> _streamBuff;
};
} // namespace LibAbyss

#endif // LIBABYSS_INPUTSTREAM_H
