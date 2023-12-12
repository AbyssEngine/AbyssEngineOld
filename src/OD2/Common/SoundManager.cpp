#include "SoundManager.h"
#include "Abyss/AbyssEngine.h"
#include "DataTableManager.h"
#include <cctype>
#include <stdexcept>
#include <absl/strings/str_cat.h>

namespace OD2::Common {

SoundManager::SoundManager() : _index(DataTableManager::getInstance().buildIndexBy("SoundSettings", "Sound")) {
}

const std::string& SoundManager::getSound(std::string_view name) {
  auto it = _cache.find(name);
  if (it != _cache.end()) {
    return it->second;
  }

  std::vector<std::string> attempts;
  const DataTable& table = DataTableManager::getInstance().getDataTable("SoundSettings");
  auto iter = _index.find(name);
  if (iter == _index.end()) {
    throw std::runtime_error(absl::StrCat("Can't find sound ", name));
  }
  const auto& mainRow = table[iter->second];
  // TODO perhaps redirect should be used only in HD mode?
  const auto redir = mainRow.find("Redirect");
  if (redir != mainRow.end() && !redir->second.empty()) {
    attempts.push_back(table[_index.at(redir->second)].at("FileName"));
    // cursor_button_click redirects to cursor_button_hd_1 but only cursor_button_3_hd.flac actually exists
    if (std::isdigit(redir->second.back())) {
      std::string start = redir->second.substr(0, redir->second.length() - 1);
      for (char i = '1'; i <= '5'; ++i) {
        start.push_back(i);
        auto itr = _index.find(start);
        if (itr != _index.end()) {
          attempts.push_back(table[itr->second].at("FileName"));
        }
        start.pop_back();
      }
    }
  }
  attempts.push_back(mainRow.at("FileName"));
  auto& instance = Abyss::AbyssEngine::getInstance();
  for (const auto& a : attempts) {
    // TODO take music vs sfx and hd vs sd from 'Channel' field, but it's only for D2R
    std::string file = "/data/hd/global/sfx/" + a;
    if (instance.fileExists(file)) {
      return _cache.emplace(name, file).first->second;
    }
    file = "/data/global/sfx/" + a;
    if (instance.fileExists(file)) {
      return _cache.emplace(name, file).first->second;
    }
    file = "/data/hd/global/music/" + a;
    if (instance.fileExists(file)) {
      return _cache.emplace(name, file).first->second;
    }
    file = "/data/global/music/" + a;
    if (instance.fileExists(file)) {
      return _cache.emplace(name, file).first->second;
    }
  }
  throw std::runtime_error(absl::StrCat("Can't find sound file for ", name));
}

} // namespace OD2::Common
