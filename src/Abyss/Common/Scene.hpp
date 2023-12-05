#pragma once

#include <SDL2/SDL.h>
#include <chrono>

namespace Abyss::Common {
class Scene {
  public:
    Scene() = default;

    virtual ~Scene() = default;

    virtual void update(std::chrono::duration<double> deltaTime) = 0;

    virtual void processEvent(const SDL_Event &event) = 0;

    virtual void render() = 0;
};
} // namespace Abyss::Common
