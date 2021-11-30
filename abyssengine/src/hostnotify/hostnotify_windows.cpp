#include "hostnotify.h"
#include <SDL.h>
#include <Windows.h>

void* AbyssEngine::HostNotify::Win32Handle;

void AbyssEngine::HostNotify::Notify(eNotifyType notifyType, const std::string &caption, const std::string &message) {

    long iconType;
    switch (notifyType) {
    default:
    case eNotifyType::Info:
        iconType = MB_ICONINFORMATION;
        break;
    case eNotifyType::Warning:
        iconType = MB_ICONWARNING;
        break;
    case eNotifyType::Fatal:
        iconType = MB_ICONERROR;
        break;
    }

    MessageBox(static_cast<HWND>(Win32Handle), message.c_str(), caption.c_str(), iconType | MB_OK);
}
