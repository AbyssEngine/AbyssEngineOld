#include "Layout.h"
#include "Abyss/Common/JSON.h"
#include "Abyss/Singletons.h"
#include <absl/container/flat_hash_map.h>
#include <absl/strings/str_cat.h>

namespace OD2::Layouts {

namespace {

void mergeFromParent(nlohmann::json &object, nlohmann::json &parent) {
    if (parent.contains("fields")) {
        nlohmann::json fields = std::move(parent["fields"]);
        if (object.contains("fields")) {
            fields.merge_patch(object["fields"]);
        }
        object["fields"] = std::move(fields);
    }
    if (object.contains("children") && parent.contains("children")) {
        absl::flat_hash_map<std::string, nlohmann::json *> brothers;
        for (auto &c : parent["children"]) {
            if (c.contains("name")) {
                brothers[c["name"].get<std::string_view>()] = &c;
            }
        }
        for (auto &c : object["children"]) {
            if (c.contains("name")) {
                auto it = brothers.find(c["name"].get<std::string_view>());
                if (it != brothers.end()) {
                    nlohmann::json &brother = *it->second;
                    mergeFromParent(c, brother);
                }
            }
        }
    }
}

nlohmann::json readMergedLayout(std::string_view name) {
    nlohmann::json me = Abyss::Common::parseJson(Abyss::Singletons::getFileProvider().loadString(absl::StrCat("/data/global/ui/layouts/", name)));
    if (me.contains("basedOn")) {
        nlohmann::json parent = readMergedLayout(me["basedOn"].get<std::string_view>());
        mergeFromParent(me, parent);
    }
    return me;
}

} // namespace

Layout::Layout(std::string_view name, const Profile &profile) {
    _data = readMergedLayout(name);
    profile.resolveReferences(_data);
}

} // namespace OD2::Layouts
