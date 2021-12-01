#ifndef LIBABYSS_CASC_H
#define LIBABYSS_CASC_H

#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <istream>
#include <filesystem>
#include <libabyss/inputstream.h>

namespace LibAbyss {

    class CASC {
    public:

        /// Proxy constructor that creates an CASC based on the specified filename.
        /// \param cascPath Path to the CASC dir to load.
        explicit CASC(const std::filesystem::path &cascPath);

        ~CASC();

        bool HasFile(std::string_view fileName);
        InputStream Load(std::string_view fileName);
        std::vector<std::string> FileList();

    private:
        void* _storage;

        std::string FixPath(std::string_view str);
    };

} // namespace LibAbyss

#endif // LIBABYSS_CASC_H
