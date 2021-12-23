#ifndef LIBABYSS_FORMATS_TBL_H
#define LIBABYSS_FORMATS_TBL_H

#include <absl/container/flat_hash_map.h>
#include <libabyss/streams/inputstream.h>
#include <string>

namespace LibAbyss {

absl::flat_hash_map<std::string, std::string> ReadTbl(InputStream stream);

}

#endif // LIBABYSS_FORMATS_TBL_H
