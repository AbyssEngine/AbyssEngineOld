#include "hostnotify.h"
#include "hostnotify_mac_shim.h"

void AbyssEngine::HostNotify::Notify(eNotifyType notifyType, const std::string &caption, const std::string &message) {
    AbyssHostNotifyMacInfo((int)notifyType, caption.c_str(), message.c_str());
}
