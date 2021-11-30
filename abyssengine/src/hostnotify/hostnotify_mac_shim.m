#include "hostnotify_mac_shim.h"
#include <AppKit/AppKit.h>


static NSApplicationActivationPolicy old_app_policy;
static bool canShowMacErrors = true;

void runOnMainQueueWithoutDeadlocking(void (^block)(void)) {
    if ([NSThread isMainThread]) {
        block();
    } else {
        dispatch_sync(dispatch_get_main_queue(), block);
    }
}

void AbyssHostNotifyInitMac() {

    NSApplication* app = [NSApplication sharedApplication];
    old_app_policy = [app activationPolicy];
    if (old_app_policy == NSApplicationActivationPolicyProhibited) {
        if (![app setActivationPolicy:NSApplicationActivationPolicyAccessory]) {
            canShowMacErrors = false;
        }
    }
}

void AbyssHostNotifyFinalizeMac(void) {
    [[NSApplication sharedApplication] setActivationPolicy:old_app_policy];
}

void AbyssHostNotifyMacInfo(int notifyType, const char *title, const char*message) {
    if (!canShowMacErrors)
        return;

    runOnMainQueueWithoutDeadlocking(^{
        NSString *m = [NSString stringWithCString:message encoding:NSASCIIStringEncoding];
        NSString *t = [NSString stringWithCString:title encoding:NSASCIIStringEncoding];
        NSAlert *alert = [NSAlert alertWithMessageText:t defaultButton:@"Ok" alternateButton:nil otherButton:nil informativeTextWithFormat:m];

        switch (notifyType) {
            default:
            case 0: // Info
                [alert setAlertStyle: NSInformationalAlertStyle];
                break;
            case 1: // Warning
                [alert setAlertStyle: NSWarningAlertStyle];
                break;
            case 2: // Error
                [alert setAlertStyle: NSAlertStyleCritical];
                break;
        }
        [alert runModal];
    });
}