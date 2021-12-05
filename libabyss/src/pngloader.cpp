#include "pngloader.h"
#include <absl/cleanup/cleanup.h>
#include <exception>
#include <memory>
#include <png.h>

#define PNG_SIGNATURE_SIZE 8

static void readPngDataStream(png_structp pngPtr, png_bytep data, png_size_t length) {
    png_voidp a = png_get_io_ptr(pngPtr);
    ((std::istream *)a)->read((char *)data, (std::streamsize)length);
}

LibAbyss::PNGLoader::PNGLoader(LibAbyss::InputStream &stream) : _pixelData() {

    png_byte pngSig[8];
    stream.read((char *)pngSig, PNG_SIGNATURE_SIZE);

    if (!stream.good())
        throw std::runtime_error("Failed to read stream while loading PNG image.");

    if (!png_sig_cmp(pngSig, 0, PNG_SIGNATURE_SIZE))
        throw std::runtime_error("Invalid signature while reading PNG image.");

    png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!pngPtr)
        throw std::runtime_error("Failed to initialize PNG structure.");

    png_infop infoPtr = png_create_info_struct(pngPtr);
    if (!infoPtr)
        throw std::runtime_error("Failed to initialize PNG info structure.");

    png_bytepp rowPointers = nullptr;
    auto rowPointersCleanup = absl::Cleanup([rowPointers] { delete[] rowPointers; });

    char *data = nullptr;
    auto cleanupData = absl::Cleanup([data] { delete[] data; });

    if (setjmp(png_jmpbuf(pngPtr))) { // NOLINT(cert-err52-cpp)
        png_destroy_read_struct(&pngPtr, &infoPtr, nullptr);
        throw std::runtime_error("Failed to load PNG.");
    }

    png_set_read_fn(pngPtr, (png_voidp)&stream, readPngDataStream);

    png_set_sig_bytes(pngPtr, PNG_SIGNATURE_SIZE);
    png_read_info(pngPtr, infoPtr);

    const auto imageWidth = png_get_image_width(pngPtr, infoPtr);
    const auto imageHeight = png_get_image_height(pngPtr, infoPtr);
    auto bitDepth = png_get_bit_depth(pngPtr, infoPtr);
    auto numChannels = png_get_channels(pngPtr, infoPtr);
    auto colorType = png_get_color_type(pngPtr, infoPtr);

    switch (colorType) {
    case PNG_COLOR_TYPE_PALETTE:
        png_set_palette_to_rgb(pngPtr);
        numChannels = 3;
        break;
    case PNG_COLOR_TYPE_GRAY:
        if (bitDepth < 8)
            png_set_expand_gray_1_2_4_to_8(pngPtr);
        bitDepth = 8;
        break;
    default:
        break;
    }

    if (png_get_valid(pngPtr, infoPtr, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(pngPtr);
        numChannels += 1;
    }

    if (bitDepth == 16) {
        png_set_strip_16(pngPtr);
        bitDepth = 8;
    }

    png_read_update_info(pngPtr, infoPtr);

    rowPointers = new png_bytep[imageHeight];

    if (bitDepth != 8)
        throw std::runtime_error("PNG must have an 8 bit depth.");

    if (numChannels != 4)
        throw std::runtime_error("PNG must convertable to RGBA format.");

    data = new char[imageWidth * imageHeight * numChannels];
    auto stride = imageWidth * numChannels;

    for (size_t i = 0; i < imageHeight; i++) {
        png_uint_32 q = (imageHeight - i - 1) * stride;
        rowPointers[i] = (png_bytep)data + q;
    }

    png_read_image(pngPtr, rowPointers);

    for (auto i = 0; i < imageWidth * imageHeight; i++) {
        const auto dataPtr = &data[i * 4];
        const auto red = dataPtr[0];
        const auto green = dataPtr[1];
        const auto blue = dataPtr[2];
        const auto alpha = dataPtr[3];

        _pixelData.emplace_back(((uint32_t)red) | ((uint32_t)green << 8) | ((uint32_t)blue << 16) | ((uint32_t)alpha << 24));
    }

    png_destroy_read_struct(&pngPtr, &infoPtr, nullptr);
}

std::span<uint32_t> LibAbyss::PNGLoader::GetPixelData() { return _pixelData; }
