#ifndef ABYSSENGINE_HOSTNOTIFY_MAC_H
#define ABYSSENGINE_HOSTNOTIFY_MAC_H

#if __cplusplus
extern "C" {
#endif

void AbyssHostNotifyInitMac();
void AbyssHostNotifyFinalizeMac(void);
void AbyssHostNotifyMacInfo(int type, const char *title, const char*message);
int AbyssHostNotifyMacCanClose(void);

#if __cplusplus
}
#endif

#endif // ABYSSENGINE_HOSTNOTIFY_MAC_H