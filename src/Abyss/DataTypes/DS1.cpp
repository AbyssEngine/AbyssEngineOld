#include "DS1.h"

#include "Abyss/AbyssEngine.h"
#include "Abyss/Streams/StreamReader.h"

#include <stdexcept>

namespace Abyss::DataTypes {

DS1::DS1(const std::string_view path) {
    if (const auto lastSeparator = std::max(path.find_last_of('/'), path.find_last_of('\\')); lastSeparator != std::string_view::npos) {
        name = std::string(path.substr(lastSeparator + 1));
    } else {
        name = std::string(path);
    }

    auto file = AbyssEngine::getInstance().loadFile(path);
    Streams::StreamReader sr(file);

    version = sr.readInt32();
    if (version < 3) {
        throw std::runtime_error("DS1 version is too old: " + std::to_string(version));
    }

    width = sr.readInt32() + 1;
    height = sr.readInt32() + 1;
    act = version >= 8 ? sr.readInt32() : 0;
    substitutionType = version >= 10 ? sr.readInt32() : 0;

    const auto numberOfFiles = sr.readInt32();

    for (int i = 0; i < numberOfFiles; ++i) {
        files.push_back(sr.readString());
    }

    int numFloors = 1;
    constexpr int numShadows = 1;
    const int numSubstitutions = (substitutionType == 1 || substitutionType == 2) ? 1 : 0;
    int numWalls = 0;

    if (version >= 9 && version <= 13)
        sr.skip(1);

    if (version >= 4)
        numWalls = sr.readInt32();

    if (version >= 16)
        numFloors = sr.readInt32();

    layers.floor.resize(numFloors);
    layers.wall.resize(numWalls);
    layers.shadow.resize(numShadows);
    layers.substitution.resize(numSubstitutions);

    resize(width, height);
}

void DS1::resize(const int width, const int height) {
    const auto elements = width * height;
    for (auto &layer : layers.floor)
        layer.resize(elements);
    for (auto &layer : layers.wall)
        layer.resize(elements);
    for (auto &layer : layers.shadow)
        layer.resize(elements);
    for (auto &layer : layers.substitution)
        layer.resize(elements);
}

} // namespace Abyss::DataTypes
