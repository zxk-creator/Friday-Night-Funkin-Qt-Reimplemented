#include "data/song/SongData.h"

using namespace std;

// ==================== 版本迁移逻辑 ====================

static SongMetaData migrate_v2_0_to_v2_2(const SongMetadata_v2_0_0& old) {
    SongMetaData m;
    m.version = "2.2.4";
    m.songName = old.songName;
    m.artist = old.artist;
    m.charter = old.charter;
    m.divisions = old.divisions;
    m.looped = old.looped;
    m.offsets = old.offsets;
    m.generatedBy = old.generatedBy;
    m.timeFormat = old.timeFormat;
    m.timeChanges = old.timeChanges;

    // 将顶层字段迁移到 playData
    m.playData.difficulties = old.difficulties;
    m.playData.characters = old.characters;
    m.playData.stage = old.stage;
    m.playData.noteStyle = old.noteStyle;
    m.playData.ratings = old.ratings;

    // 2.2.x 新增字段，设置默认值
    m.playData.songVariations = {};
    m.playData.previewStart = 0;
    m.playData.previewEnd = 15000;
    m.playData.album = std::nullopt;
    m.playData.stickerPack = std::nullopt;

    return m;
}

static SongMetaData migrate_v2_1_to_v2_2(const SongMetadata_v2_1_0& old) {
    SongMetaData m;
    m.version = "2.2.4";
    m.songName = old.songName;
    m.artist = old.artist;
    m.charter = old.charter;
    m.divisions = old.divisions;
    m.looped = old.looped;
    m.offsets = old.offsets;
    m.playData = old.playData;
    m.generatedBy = old.generatedBy;
    m.timeFormat = old.timeFormat;
    m.timeChanges = old.timeChanges;

    // 补全 2.2.x 新增的字段（如果缺失）
    if (!m.playData.album.has_value()) {
        m.playData.album = std::nullopt;
    }
    if (!m.playData.stickerPack.has_value()) {
        m.playData.stickerPack = std::nullopt;
    }

    return m;
}

static SongMetaData parseMetadata_v2_2_x(const json& j) {
    SongMetaData m;
    m.from_json(j);
    return m;
}

void SongMetadata_v2_1_0::from_json(const json& j)
{
    if (j.contains("version")) version = QString::fromStdString(j["version"].get<std::string>());
    if (j.contains("songName")) songName = QString::fromStdString(j["songName"].get<std::string>());
    if (j.contains("artist")) artist = QString::fromStdString(j["artist"].get<std::string>());
    if (j.contains("charter")) charter = QString::fromStdString(j["charter"].get<std::string>());
    if (j.contains("divisions")) divisions = j["divisions"].get<int>();
    if (j.contains("looped")) looped = j["looped"].get<bool>();
    if (j.contains("offsets")) offsets.from_json(j["offsets"]);
    if (j.contains("playData")) playData.from_json(j["playData"]);
    if (j.contains("generatedBy")) generatedBy = QString::fromStdString(j["generatedBy"].get<std::string>());
    if (j.contains("timeFormat")) {
        std::string s = j["timeFormat"].get<std::string>();
        if (s == "ticks") timeFormat = SongTimeFormat::TICKS;
        else if (s == "float") timeFormat = SongTimeFormat::FLOAT;
        else timeFormat = SongTimeFormat::MILLISECONDS;
    }
    if (j.contains("timeChanges") && j["timeChanges"].is_array()) {
        timeChanges.clear();
        for (const auto& item : j["timeChanges"]) {
            SongTimeChange t;
            t.from_json(item);
            timeChanges.push_back(t);
        }
    }
}

void SongChartData::from_json(const json& j)
{
    // version
    if (j.contains("version")) {
        version = QString::fromStdString(j["version"].get<std::string>());
    }

    // generatedBy
    if (j.contains("generatedBy")) {
        generatedBy = QString::fromStdString(j["generatedBy"].get<std::string>());
    }

    // scrollSpeed: Map<String, Float>
    if (j.contains("scrollSpeed") && j["scrollSpeed"].is_object()) {
        for (auto& [key, val] : j["scrollSpeed"].items()) {
            scrollSpeed[QString::fromStdString(key)] = val.get<double>();
        }
    }

    // events: Array<SongEventData>
    if (j.contains("events") && j["events"].is_array()) {
        events.clear();
        for (const auto& item : j["events"]) {
            SongEventData e;
            e.from_json(item);
            events.push_back(e);
        }
    }

    // notes: Map<String, Array<SongNoteData>>
    // 我们也不用手动操作Difficulty结构了，直接一键解析完成了
    if (j.contains("notes") && j["notes"].is_object()) {
        for (auto& [difficulty, notesArray] : j["notes"].items()) {
            if (notesArray.is_array()) {
                QVector<SongNoteData> noteVec;
                for (const auto& noteJson : notesArray) {
                    SongNoteData note;
                    note.from_json(noteJson);
                    noteVec.push_back(note);
                }
                difficulties.notes[QString::fromStdString(difficulty)] = noteVec;
            }
        }
    }

    // variation 不在 JSON 中，由代码填充，用于存储到Song对象的difficulty里面
}

std::optional<SongMetaData> SongDataParser::parseSongMetaData(const json& j, const QString& filename) {
    semver::version current_version;
    std::string versionStr = "2.2.4";

    if (j.contains("version") && j["version"].is_string()) {
        versionStr = j["version"].get<std::string>();
    }

    auto parseResult = semver::parse(versionStr, current_version);
    if (!parseResult) {
        Exception::logVersionInvalid(QString::fromStdString(versionStr), SongDataRelative::SONG_METADATA_VERSION_RULE, filename);
        return std::nullopt;
    }

    semver::version v2_3_0, v2_1_0, v2_2_0, v2_0_0;
    semver::parse("2.2.0", v2_2_0);
    semver::parse("2.3.0", v2_3_0);
    semver::parse("2.1.0", v2_1_0);
    semver::parse("2.0.0", v2_0_0);

    // 根据版本路由
    if (current_version >= v2_2_0 && current_version < v2_3_0) {
        // 2.2.x：直接解析
        return parseMetadata_v2_2_x(j);
    }
    else if (current_version >= v2_1_0 && current_version < v2_2_0) {
        // 2.1.x：解析旧结构到迁移
        SongMetadata_v2_1_0 old;
        old.from_json(j);
        return migrate_v2_1_to_v2_2(old);
    }
    else if (current_version >= v2_0_0 && current_version < v2_1_0) {
        // 2.0.x：解析旧结构 → 迁移
        SongMetadata_v2_0_0 old;
        old.from_json(j);
        return migrate_v2_0_to_v2_2(old);
    }
    else {
        // 不支持的版本
        Exception::logVersionInvalid(QString::fromStdString(versionStr), SongDataRelative::SONG_METADATA_VERSION_RULE, filename);
        return std::nullopt;
    }
}

SongChartData SongDataParser::parseSongChartData(const json& j, const QString& filename) {
    SongChartData s;
    s.from_json(j);
    return s;
}