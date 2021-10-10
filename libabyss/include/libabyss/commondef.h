#ifndef LIBABYSS_COMMONDEF_H
#define LIBABYSS_COMMONDEF_H

#ifdef __GNUC__
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif

#endif // LIBABYSS_COMMONDEF_H