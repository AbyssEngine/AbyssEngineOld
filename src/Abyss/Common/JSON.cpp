#include "JSON.h"
#include <format>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <stdexcept>

namespace Abyss::Common {

nlohmann::json parseJson(std::string_view json) {
    rapidjson::Document d;
    d.Parse<rapidjson::kParseCommentsFlag | rapidjson::kParseTrailingCommasFlag>(json.data(), json.length());
    if (d.HasParseError()) {
        throw std::runtime_error(
            std::format("JSON parse error ({}) at offset {}", rapidjson::GetParseError_En(d.GetParseError()), d.GetErrorOffset()));
    }
    rapidjson::StringBuffer sb;
    rapidjson::Writer writer(sb);
    d.Accept(writer);
    return nlohmann::json::parse(sb.GetString());
}

} // namespace Abyss::Common
