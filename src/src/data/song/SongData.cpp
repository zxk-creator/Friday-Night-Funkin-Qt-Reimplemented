#include "data/song/SongData.h"

SongTimeFormat SongDataParser::SongTimeFormat_from_json_VS(const json& j)
{
    std::string s = j.get<std::string>();
    if (s == "ticks") return SongTimeFormat::TICKS;
    if (s == "float") return SongTimeFormat::FLOAT;
    return SongTimeFormat::MILLISECONDS;
}

SongCharacterData SongDataParser::SongCharacterData_from_json_VS(const json& j)
{
    SongCharacterData c;
    if (j.contains("player")) c.player = j["player"].get<std::string>();
    if (j.contains("girlfriend")) c.girlfriend = j["girlfriend"].get<std::string>();
    if (j.contains("opponent")) c.opponent = j["opponent"].get<std::string>();
    if (j.contains("instrumental")) c.instrumental = j["instrumental"].get<std::string>();
    if (j.contains("altInstrumentals")) {
        c.altInstrumentals = j["altInstrumentals"].get<std::vector<std::string>>();
    }
    if (j.contains("opponentVocals")) {
        c.opponentVocals = j["opponentVocals"].get<std::vector<std::string>>();
    }
    if (j.contains("playerVocals")) {
        c.playerVocals = j["playerVocals"].get<std::vector<std::string>>();
    }
    return c;
}

SongPlayData SongDataParser::SongPlayData_from_json_VS(const json& j)
{
    SongPlayData p;
    if (j.contains("songVariations")) {
        p.songVariations = j["songVariations"].get<std::vector<std::string>>();
    }
    if (j.contains("difficulties")) {
        p.difficulties = j["difficulties"].get<std::vector<std::string>>();
    }
    if (j.contains("characters")) {
        p.characters = SongCharacterData_from_json_VS(j["characters"]);
    }
    if (j.contains("stage")) p.stage = j["stage"].get<std::string>();
    if (j.contains("noteStyle")) p.noteStyle = j["noteStyle"].get<std::string>();
    if (j.contains("ratings")) {
        for (auto& [key, val] : j["ratings"].items()) {
            p.ratings[key] = val.get<int>();
        }
    }
    if (j.contains("album")) p.album = j["album"].get<std::string>();
    if (j.contains("stickerPack")) p.stickerPack = j["stickerPack"].get<std::string>();
    if (j.contains("previewStart")) p.previewStart = j["previewStart"].get<int>();
    if (j.contains("previewEnd")) p.previewEnd = j["previewEnd"].get<int>();
    return p;
}

SongMetadata_v2_0_0 SongDataParser::SongMetadata_v2_0_0_from_json(const json& j)
{
    SongMetadata_v2_0_0 m;

    if (j.contains("version")) m.version = j["version"].get<std::string>();
    if (j.contains("songName")) m.songName = j["songName"].get<std::string>();
    if (j.contains("artist")) m.artist = j["artist"].get<std::string>();
    if (j.contains("charter")) m.charter = j["charter"].get<std::string>();
    if (j.contains("divisions")) m.divisions = j["divisions"].get<int>();
    if (j.contains("looped")) m.looped = j["looped"].get<bool>();
    if (j.contains("offsets")) m.offsets = SongOffsets_from_json_VS(j["offsets"]);

    // 2.0.x 特有：字段在顶层
    if (j.contains("difficulties")) {
        m.difficulties = j["difficulties"].get<std::vector<std::string>>();
    }
    if (j.contains("characters")) {
        m.characters = SongCharacterData_from_json_VS(j["characters"]);
    }
    if (j.contains("stage")) m.stage = j["stage"].get<std::string>();
    if (j.contains("noteStyle")) m.noteStyle = j["noteStyle"].get<std::string>();
    if (j.contains("ratings")) {
        for (auto& [key, val] : j["ratings"].items()) {
            m.ratings[key] = val.get<int>();
        }
    }

    if (j.contains("generatedBy")) m.generatedBy = j["generatedBy"].get<std::string>();
    if (j.contains("timeFormat")) m.timeFormat = SongTimeFormat_from_json_VS(j["timeFormat"]);
    if (j.contains("timeChanges") && j["timeChanges"].is_array()) {
        for (const auto& item : j["timeChanges"]) {
            m.timeChanges.push_back(SongTimeChange_from_json_VS(item));
        }
    }

    return m;
}
