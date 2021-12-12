#ifndef LIBABYSS_ZONEPROVIDERS_H
#define LIBABYSS_ZONEPROVIDERS_H

#include <functional>
#include <string>
#include <libabyss/formats/d2/dt1.h>
#include <libabyss/formats/d2/ds1.h>

namespace LibAbyss {
    typedef std::function<DT1(std::string_view)> ProvideDT1Handler;
    typedef std::function<DS1(std::string_view)> ProvideST1Handler;

}

#endif // LIBABYSS_ZONEPROVIDERS_H