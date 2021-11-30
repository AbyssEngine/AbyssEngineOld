#ifndef ABYSS_ALIGNMENT_H
#define ABYSS_ALIGNMENT_H

#include <string>

namespace AbyssEngine {

enum class eAlignment { Unknown, Start, Middle, End };

std::string_view AlignmentToString(const eAlignment &alignment);
eAlignment StringToAlignment(std::string_view str);

} // namespace AbyssEngine

#endif // ABYSS_ALIGNMENT_H
