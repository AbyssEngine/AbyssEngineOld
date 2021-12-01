#ifndef ABYSS_ABYSSSPRITE_H
#define ABYSS_ABYSSSPRITE_H

#include "inputstream.h"
#include "inifile.h"
#include <vector>

namespace LibAbyss {

class AbyssSprite {
  public:
    class Animation {

    };

    explicit AbyssSprite(InputStream &imageStream, const INIFile &ini);

    std::vector<Animation> Animations;

};

} // namespace LibAbyss

#endif // ABYSS_ABYSSSPRITE_H
