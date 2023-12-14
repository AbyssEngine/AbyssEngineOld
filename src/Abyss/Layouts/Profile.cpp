#include "Profile.h"
#include "Abyss/Common/JSON.h"
#include "Abyss/Singletons.h"
#include <format>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace Abyss::Layouts {

namespace {

nlohmann::json readMergedProfile(std::string_view name) {
    nlohmann::json me =
        Abyss::Common::parseJson(Singletons::getFileProvider().loadString(std::format("/data/global/ui/layouts/_profile{}.json", name)));
    if (me.contains("basedOn")) {
        nlohmann::json base = readMergedProfile(me["basedOn"].get<std::string_view>());
        base.merge_patch(me);
        return base;
    }
    return me;
}

bool resolveDataReferences(nlohmann::json &object, const nlohmann::json &profile) {
    bool again = false;
    for (auto &[k, v] : object.items()) {
        if (v.is_structured()) {
            again = resolveDataReferences(v, profile) || again;
        } else if (v.is_string() && v.get<std::string_view>().starts_with('$')) {
            v = profile.at(v.get<std::string_view>().substr(1));
            again = true;
        }
    }
    return again;
}

} // namespace

Profile::Profile(std::string_view name) {
    _data = readMergedProfile(name);
    while (resolveDataReferences(_data, _data)) {
    }
}

void Profile::resolveReferences(nlohmann::json& object) const {
    resolveDataReferences(object, _data);
}

} // namespace Abyss::Layouts
