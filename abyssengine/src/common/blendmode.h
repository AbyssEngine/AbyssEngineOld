#ifndef ABYSS_BLENDMODE_H
#define ABYSS_BLENDMODE_H

#include <string>

namespace AbyssEngine {

enum class eBlendMode { Unknown, None, Blend, Add, Mod, Mul };

std::string_view BlendModeToString(const eBlendMode &blendMode);
eBlendMode StringToBlendMode(std::string_view str);
} // namespace AbyssEngine

#endif // ABYSS_BLENDMODE_H
