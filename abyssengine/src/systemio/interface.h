#ifndef ABYSS_SYSTEMIO_INTERFACE_H
#define ABYSS_SYSTEMIO_INTERFACE_H

#include "../common/blendmode.h"
#include "../common/rectangle.h"
#include "libabyss/inputstream.h"
#include "../common/events.h"
#include <memory>
#include <mutex>
#include <span>
#include <string>
#include <vector>
#include <bitset>

namespace AbyssEngine {

class Sprite;
class Node;

class ITexture {
  public:
    enum class Format { Static, YUV };

    virtual ~ITexture() = default;
    virtual void SetPixels(std::span<const uint32_t> pixels) = 0;
    virtual void SetYUVData(std::span<uint8_t> yPlane, int yPitch, std::span<uint8_t> uPlane, int uPitch, std::span<uint8_t> vPlane, int vPitch) = 0;
    virtual void Render(const AbyssEngine::Rectangle &sourceRect, const AbyssEngine::Rectangle &destRect) = 0;
    virtual void SetBlendMode(eBlendMode blendMode) = 0;
    virtual void SetColorMod(uint8_t red, uint8_t green, uint8_t blue) = 0;
    virtual eBlendMode GetBlendMode() = 0;
};

class SystemIO {
  public:
    virtual ~SystemIO() = default;
    virtual std::string_view Name() = 0;

    /// Pauses the audio subsystem.
    /// \param pause Pauses the audio if true, resumes if false.
    virtual void PauseAudio(bool pause) = 0;

    /// Sets the window state to full screen.
    /// \param fullscreen Window is set to fullscreen if true, or windowed if false.
    virtual void SetFullscreen(bool fullscreen) = 0;

    /// Starts the main system loop.
    virtual void RunMainLoop(Node &rootNode) = 0;

    /// Stops the main loop
    virtual void Stop() = 0;

    /// Creates a new texture.
    /// \param textureFormat The format of the texture.
    /// \param width The width of the texture.
    /// \param height The height of the texture.
    /// \return A new texture instance.
    virtual std::unique_ptr<ITexture> CreateTexture(ITexture::Format textureFormat, uint32_t width, uint32_t height) = 0;

    /// Writes data to the audio buffer.
    /// \param data The data to write to the audio buffer.
    virtual void PushAudioData(std::span<uint8_t> data) = 0;

    /// Plays the specified video.
    /// \param stream The video stream
    /// \param wait If true, allows WaitForVideoToFinish() to halt thread before returning.
    virtual void PlayVideo(LibAbyss::InputStream stream, bool wait) = 0;

    /// When video is playing with wait set to true, this function will wait for video playback to complete.
    virtual void WaitForVideoToFinish() = 0;

    /// Resets all audio buffers.
    virtual void ResetAudio() = 0;

    /// Sets the cursor sprite (or clears if null)
    /// \param cursorSprite The sprite to use for the cursor.
    /// \param offsetX X offset from origin
    /// \param offsetY Y offset from origin
    void SetCursorSprite(Sprite *cursorSprite, int offsetX, int offsetY);

    /// Hides or shows the cursor.
    /// \param show True to show the cursor, otherwise false.
    void ShowSystemCursor(bool show);

    /// Returns the cursor position.
    /// \param x This reference is set to the X cursor position.
    /// \param y This reference is set to the Y cursor position.
    void GetCursorPosition(int &x, int &y) const;

    /// Gets the mouse button state.
    /// \return The mouse button state.
    std::bitset<3> GetMouseButtonState();

    /// Resets the mouse button state.
    void ResetMouseButtonState();

  protected:
    Sprite *_cursorSprite = nullptr;
    bool _showSystemCursor = false;
    int _cursorX = 0;
    int _cursorY = 0;
    int _cursorOffsetX = 0;
    int _cursorOffsetY = 0;
    std::bitset<3> _mouseButtonState;

    std::mutex _mutex;
};

} // namespace AbyssEngine

#endif // ABYSS_SYSTEMIO_INTERFACE_H
