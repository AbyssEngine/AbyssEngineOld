#include <absl/strings/str_format.h>
#include <spdlog/spdlog.h>
#include <ios>
#include <libabyss/formats/d2/tbl.h>
#include <libabyss/streams/streamreader.h>
#include <string_view>

namespace LibAbyss {

absl::flat_hash_map<std::string, std::string> ReadTbl(InputStream stream) {
    // https://d2mods.info/forum/viewtopic.php?p=202077#p202077
    StreamReader sr(stream);
    sr.ReadUInt16(); // crc
    uint16_t num = sr.ReadUInt16();
    uint32_t hashTableSize = sr.ReadUInt32();
    /*uint8_t version = */ sr.ReadUInt8();
    uint32_t offsetStart = sr.ReadUInt32();
    /*uint32_t maxTries = */ sr.ReadUInt32();
    uint32_t offsetEnd = sr.ReadUInt32();
    /*std::vector<uint16_t> indices;
    indices.reserve(num);*/
    for (int i = 0; i < num; ++i) {
        // indices.push_back
        sr.ReadUInt16();
    }
    struct Record {
        uint8_t used;
        uint16_t index;
        /*uint32_t hash;*/
        uint32_t keyOffset;
        uint32_t valueOffset;
        uint16_t valueLen;
    };
    std::vector<Record> records;
    records.reserve(hashTableSize);
    for (uint32_t i = 0; i < hashTableSize; ++i) {
        records.emplace_back();
        Record &record = records.back();
        record.used = sr.ReadUInt8();
        record.index =  sr.ReadUInt16();
        /*record.hash = */ sr.ReadUInt32();
        record.keyOffset = sr.ReadUInt32();
        record.valueOffset = sr.ReadUInt32();
        record.valueLen = sr.ReadUInt16();
    }
    stream.seekg(offsetStart, std::ios_base::beg);
    std::string raw_data;
    raw_data.resize(offsetEnd - offsetStart);
    stream.read(raw_data.data(), offsetEnd - offsetStart);
    if (stream.gcount() != raw_data.size()) {
        throw std::runtime_error("Couldn't read the whole tbl data");
    }
    std::string_view raw_data_view = raw_data;
    absl::flat_hash_map<std::string, std::string> result;
    for (const Record &r : records) {
        if (!r.used)
            continue;
        if (r.keyOffset < offsetStart || r.keyOffset >= offsetEnd || r.valueOffset < offsetStart || r.valueOffset >= offsetEnd)
            throw std::runtime_error("invalid offset in tbl record");
        std::string_view key_view = raw_data_view.substr(r.keyOffset - offsetStart);
        std::string key(key_view.substr(0, key_view.find_first_of('\0')));
        std::string_view value_view = raw_data_view.substr(r.valueOffset - offsetStart, (int)r.valueLen - 1);
        result[std::move(key)] = std::string(value_view);
        result[absl::StrCat("#", r.index)] = std::string(value_view);
    }
    return result;
}

} // namespace LibAbyss
