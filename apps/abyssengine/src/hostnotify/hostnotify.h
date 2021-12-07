#ifndef ABYSSENGINE_HOSTNOTIFY_H
#define ABYSSENGINE_HOSTNOTIFY_H

#include <string>

namespace AbyssEngine {

enum class eNotifyType { Info, Warning, Fatal };

class HostNotify {
  public:
#ifdef _WIN32
    static void* Win32Handle;
#endif // _WIN32
    static void Notify(eNotifyType notifyType, const std::string &caption, const std::string &message);
};
} // namespace AbyssEngine

#endif // ABYSSENGINE_HOSTNOTIFY_H
