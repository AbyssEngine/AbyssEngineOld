#ifndef ABYSS_SYSTEMIO_INTERFACE_H
#define ABYSS_SYSTEMIO_INTERFACE_H

#include "../common/blendmode.h"
#include "../common/events.h"
#include "../common/rectangle.h"
#include "../common/audiointent.h"
#include "libabyss/audiostream.h"
#include "../engine/soundeffect.h"
#include <memory>
#include <mutex>
#include <span>
#include <string>
#include <vector>

namespace AbyssEngine {

class Sprite;
class Node;

class ITexture {
  public:
    enum class Format { Static, YUV };

    virtual ~ITexture() = default;
    virtual void SetPixels(std::span<const uint32_t> pixels) = 0;
    virtual void SetPixels(std::span<const uint8_t> pixels) = 0;
    virtual void SetYUVData(std::span<const uint8_t> yPlane, int yPitch, std::span<const uint8_t> uPlane, int uPitch, std::span<const uint8_t> vPlane,
                            int vPitch) = 0;
    virtual void Render(const AbyssEngine::Rectangle &sourceRect, const AbyssEngine::Rectangle &destRect) = 0;
    virtual void SetBlendMode(eBlendMode blendMode) = 0;
    virtual void SetColorMod(uint8_t red, uint8_t green, uint8_t blue) = 0;
    virtual eBlendMode GetBlendMode() = 0;
};

/// Interface for the host subsystems.
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

    /// Creates a new texture.
    /// \param textureFormat The format of the texture.
    /// \param width The width of the texture.
    /// \param height The height of the texture.
    /// \return A new texture instance.
    virtual std::unique_ptr<ITexture> CreateTexture(ITexture::Format textureFormat, uint32_t width, uint32_t height) = 0;

    /// Writes data to the audio buffer.
    /// \param data The data to write to the audio buffer.
    virtual void PushAudioData(eAudioIntent intent, std::span<uint8_t> data) = 0;

    /// Resets all audio buffers.
    virtual void ResetAudio() = 0;

    /// Handles input events from the host system.
    /// \returns True if the engine should continue, false if it should halt.
    virtual bool HandleInputEvents(Node &rootNode) = 0;

    /// Returns the system ticks
    virtual uint32_t GetTicks() = 0;

    /// Starts the rendering process (and clears the render buffer)
    virtual void RenderStart() = 0;

    // Ends the rendering process (and draws to the screen)
    virtual void RenderEnd() = 0;

    /// Delays execution
    /// \param ms The number of milliseconds to delay.
    virtual void Delay(uint32_t ms) = 0;

    /// Returns the cursor state.
    /// \param cursorX The X coordinate of the cursor.
    /// \param cursorY The Y coordinate of the cursor.
    /// \param buttonState The button state.
    virtual void GetCursorState(int &cursorX, int &cursorY, eMouseButton &buttonState) = 0;

    /// Returns the volume.
    virtual float GetAudioLevel(eAudioIntent intent) = 0;

    /// Sets the volume.
    /// \param intent The intent of the audio.
    /// \param level The new volume.
    virtual void SetAudioLevel(eAudioIntent intent, float level) = 0;

    /// Resets the mouse button state.
    virtual void ResetMouseButtonState() = 0;

    /// Sets the background music.
    /// \param stream The stream to play.
    virtual void SetBackgroundMusic(std::unique_ptr<LibAbyss::AudioStream> stream) = 0;

    /// Adds a sound effect to the sound effects list.
    /// \param soundEffect The sound effect to add.
    virtual void AddSoundEffect(SoundEffect *soundEffect) = 0;

    /// Removes a sound effect from the sound effects list.
    /// \param soundEffect The sound effect to remove.
    virtual void RemoveSoundEffect(SoundEffect *soundEffect) = 0;

};

} // namespace AbyssEngine

#endif // ABYSS_SYSTEMIO_INTERFACE_H
