#include "libabyss/formats/d2/casc.h"
#include "libabyss/streams/cascstream.h"
#include <absl/strings/str_cat.h>
#include <absl/strings/str_format.h>
#include <absl/strings/strip.h>
#include <memory>
#include <spdlog/spdlog.h>
#include <string>

#define CASCLIB_NO_AUTO_LINK_LIBRARY 1
#include <CascLib.h>

namespace LibAbyss {

namespace {

static bool casc_progress_callback(void *PtrUserParam, LPCSTR szWork, LPCSTR szObject, DWORD CurrentValue, DWORD TotalValue) {
    if (szObject) {
        if (TotalValue) {
            SPDLOG_TRACE("CASC progress: {0} ({1}), {2}/{3}", szWork, szObject, CurrentValue, TotalValue);
        } else {
            SPDLOG_TRACE("CASC progress: {0} ({1})", szWork, szObject);
        }
    } else {
        if (TotalValue) {
            SPDLOG_TRACE("CASC progress: {0}, {1}/{2}", szWork, CurrentValue, TotalValue);
        } else {
            SPDLOG_TRACE("CASC progress: {0}", szWork);
        }
    }
    return false;
}

} // namespace

CASC::CASC(const std::filesystem::path &cascPath) {
    std::string path = std::filesystem::absolute(cascPath).string();
    CASC_OPEN_STORAGE_ARGS args = {};
    args.Size = sizeof(CASC_OPEN_STORAGE_ARGS);
    args.PfnProgressCallback = casc_progress_callback;
    if (!CascOpenStorageEx(path.c_str(), &args, 0, &_storage)) {
        throw std::runtime_error(absl::StrFormat("Error occurred while opening CASC %s: %d", cascPath.string(), GetCascError()));
    }
}

CASC::~CASC() { CascCloseStorage(_storage); }

InputStream CASC::Load(std::string_view fileName) { return InputStream(std::make_unique<CASCStream>(_storage, FixPath(fileName))); }

bool CASC::HasFile(std::string_view fileName) {
    HANDLE file;
    if (CascOpenFile(_storage, FixPath(fileName).c_str(), 0, CASC_OPEN_BY_NAME, &file)) {
        CascCloseFile(file);
        return true;
    }
    return false;
}

std::vector<std::string> CASC::FileList() {
    std::vector<std::string> result;
    return result;
}

std::string CASC::FixPath(std::string_view str) {
    absl::ConsumePrefix(&str, "/");
    absl::ConsumePrefix(&str, "\\");
    return absl::StrCat("data:", str);
}

} // namespace LibAbyss
