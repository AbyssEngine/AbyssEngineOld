#ifndef LIBABYSS_MPQ_H
#define LIBABYSS_MPQ_H

#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <istream>
#include <filesystem>
#include <libabyss/inputstream.h>

namespace LibAbyss {

    class MPQ {
    public:

        /// Proxy constructor that creates an MPQ based on the specified filename.
        /// \param mpqPath Path to the MPQ file to load.
        explicit MPQ(const std::filesystem::path &mpqPath);

        ~MPQ();

        bool HasFile(std::string_view fileName);
        InputStream Load(std::string_view fileName);
        std::vector<std::string> FileList();

    private:
        void* _stormMpq;
        std::string _mpqPath;

        static std::string FixPath(std::string_view str);
    };

} // namespace LibAbyss

#endif // LIBABYSS_MPQ_H
