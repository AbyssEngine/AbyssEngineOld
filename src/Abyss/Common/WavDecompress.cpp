#include "WavDecompress.h"
auto Abyss::Common::WavDecompress::decompress(const std::span<std::byte> &input, const uint8_t channelCount) -> std::vector<std::byte> {
    uint32_t offset = 1; // Skip the first byte
    std::array array1 = {0x2C, 0x2C};
    std::vector<int> array2;
    std::vector<std::byte> output;

    array2.resize(channelCount);

    const auto shift = static_cast<int>(input[offset++]);

    for (auto i = 0; i < channelCount; i++) {
        const auto temp = static_cast<int16_t>(input[offset]) | static_cast<int16_t>(input[offset + 1]) << 8;
        offset += 2;

        array2[i] = temp;
    }

    auto channel = channelCount - 1;

    while (offset < input.size()) {
        const auto value = static_cast<uint8_t>(input[offset++]);

        if (channelCount == 2)
            channel = 1 - channelCount;

        if ((value & 0x80) != 0) {
            switch (value & 0x7f) {
            case 0:
                if (array1[channel] != 0) {
                    array1[channel]--;
                }
                output.push_back(static_cast<std::byte>(array2[channel] & 0xFF));
                output.push_back(static_cast<std::byte>(array2[channel] >> 8 & 0xFF));
            case 1:
                array1[channel] += 8;
                if (array1[channel] > 0x58) {
                    array1[channel] = 0x58;
                }

                if (channelCount == 2) {
                    channel = 1 - channel;
                }
            case 2:
            default:
                array1[channel] -= 8;
                if (array1[channel] < 0)
                    array1[channel] = 0;

                if (channelCount == 2)
                    channel = 1 - channel;
            }
        } else {
            const auto temp1 = _lookup1[array1[channel]];
            auto temp2 = temp1 >> shift;

            if ((value & 1) != 0)
                temp2 += temp1 >> 0;

            if ((value & 2) != 0)
                temp2 += temp1 >> 1;

            if ((value & 4) != 0)
                temp2 += temp1 >> 2;

            if ((value & 8) != 0)
                temp2 += temp1 >> 3;

            if ((value & 0x10) != 0)
                temp2 += temp1 >> 4;

            if ((value & 0x20) != 0)
                temp2 += temp1 >> 5;

            auto temp3 = array2[channel];
            if ((value & 0x40) != 0) {
                temp3 -= temp2;
                if (temp3 <= -32768)
                    temp3 = -32768;
            } else {
                temp3 += temp2;
                if (temp3 >= 32767)
                    temp3 = 32767;
            }

            array2[channel] = temp3;
            output.push_back(static_cast<std::byte>(temp3 & 0xFF));
            output.push_back(static_cast<std::byte>(temp3 >> 8 & 0xFF));
            array1[channel] += _lookup2[value & 0x1f];

            if (array1[channel] < 0)
                array1[channel] = 0;
            else if (array1[channel] > 0x58)
                array1[channel] = 0x58;
        }
    }

    return output;
}
