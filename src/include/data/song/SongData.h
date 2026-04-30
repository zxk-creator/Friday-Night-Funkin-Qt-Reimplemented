//
// Created by kkplay on 4/25/26.
//

#pragma once

#include "nlohmann/json.hpp"
#include <optional>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <mutex>

#include "Constants.h"
#include "semver/semver.h"
#include "utils/exception/CustomException.h"

using json = nlohmann::json;

enum class SongTimeFormat {
    TICKS,
    FLOAT,
    MILLISECONDS
};

struct SongTimeChange {
    double timeStamp = 0.0;
    double beatTime = 0.0;
    double bpm = 100.0;
    int timeSignatureNum = 4;
    int timeSignatureDen = 4;
    std::vector<int> beatTuplets = {4, 4, 4, 4};

    SongTimeChange() = default;
    SongTimeChange(double t, double bpm) : timeStamp(t), bpm(bpm) {}
};

struct SongOffsets {
    double instrumental = 0.0;
    std::unordered_map<std::string, double> altInstrumentals;
    std::unordered_map<std::string, double> vocals;
    std::unordered_map<std::string, std::unordered_map<std::string, double>> altVocals;

    double getInstrumentalOffset(const std::string& instrumentalId = "") const {
        if (instrumentalId.empty()) return instrumental;
        auto it = altInstrumentals.find(instrumentalId);
        return (it != altInstrumentals.end()) ? it->second : instrumental;
    }

    double getVocalOffset(const std::string& charId, const std::string& instrumentalId = "") const {
        if (!instrumentalId.empty()) {
            auto itInst = altVocals.find(instrumentalId);
            if (itInst != altVocals.end()) {
                auto itChar = itInst->second.find(charId);
                if (itChar != itInst->second.end()) return itChar->second;
            }
        }
        auto it = vocals.find(charId);
        return (it != vocals.end()) ? it->second : 0.0;
    }
};

struct SongCharacterData {
    std::string player = "bf";
    std::string girlfriend = "gf";
    std::string opponent = "dad";
    std::string instrumental = "";
    // 所谓“替代伴奏”到底是什么意思
    // 我认为特可能是为erect什么准备的，不知道，有人反馈了再说吧
    std::vector<std::string> altInstrumentals;
    // 据说这个数组里面的音效会同时播放
    std::vector<std::string> opponentVocals;
    // 也会同时播放
    std::vector<std::string> playerVocals;

    SongCharacterData() = default;
    SongCharacterData(const std::string& p, const std::string& gf, const std::string& opp)
        : player(p), girlfriend(gf), opponent(opp) {}
};

/**
 * 形如
* "playData": {
    "songVariations": ["pico"],
    "difficulties": [
      "easy",
      "normal",
      "hard"
    ],
    "characters": {
      "player": "bf-redux",
      "girlfriend": "gf-redux",
      "opponent": "modsky-annoyed",
      "instrumental": "",
      "altInstrumentals": [      ],
      "opponentVocals": [
        "sky"
      ],
      "playerVocals": [
        "bf"
      ]
    }
 * 就叫playData，包含了难度数据（只有字符串）
 */
struct SongPlayData {
    // 传说中的变体，-erect，-pico等
    std::vector<std::string> songVariations;
    std::vector<std::string> difficulties;
    SongCharacterData characters;
    std::string stage = "mainStage";
    std::string noteStyle = "funkin";
    std::unordered_map<std::string, int> ratings;
    std::optional<std::string> album;
    std::optional<std::string> stickerPack;
    int previewStart = 0;
    int previewEnd = 15000;
};

/**
 * 包含PlayData，CharacterData（定义对手，玩家，女朋友是谁），Offsets等
 */
struct SongMetaData {
    std::string version = "2.2.4";
    std::string songName = "Unknown";
    std::string artist = "Unknown";
    std::optional<std::string> charter;
    std::optional<int> divisions = 96;
    bool looped = false;
    SongOffsets offsets;
    SongPlayData playData;
    std::string generatedBy;
    SongTimeFormat timeFormat = SongTimeFormat::MILLISECONDS;
    std::vector<SongTimeChange> timeChanges;

    SongMetaData() = default;
};

// ==================== 2.0.x 临时结构体 ====================
struct SongMetadata_v2_0_0 {
    std::string version;
    std::string songName;
    std::string artist;
    std::optional<std::string> charter;
    std::optional<int> divisions = 96;
    bool looped = false;
    SongOffsets offsets;

    // 2.0.x 特有：字段在顶层
    std::vector<std::string> difficulties;
    SongCharacterData characters;
    std::string stage = "mainStage";
    std::string noteStyle = "funkin";
    std::unordered_map<std::string, int> ratings;

    std::string generatedBy;
    SongTimeFormat timeFormat = SongTimeFormat::MILLISECONDS;
    std::vector<SongTimeChange> timeChanges;
};

// ==================== 2.1.x 临时结构体 ====================
struct SongMetadata_v2_1_0 {
    std::string version;
    std::string songName;
    std::string artist;
    std::optional<std::string> charter;
    std::optional<int> divisions = 96;
    bool looped = false;
    SongOffsets offsets;
    SongPlayData playData;  // 2.1.x 已有 playData，但可能缺少 album/stickerPack
    std::string generatedBy;
    SongTimeFormat timeFormat = SongTimeFormat::MILLISECONDS;
    std::vector<SongTimeChange> timeChanges;
};

struct NoteParamData {
    std::string name;
    // value: Dynamic
    json value;
};

/**
 * 什么叫NoteData，比如一个
 * {
        "t": 48900,
        "d": 5,
        "l": 225,
        "p": [        ]
      },
 * 就叫notedata，专门记录这个箭头方向和类型的
 */
struct SongNoteData {
    double time = 0.0;
    int data = 0;
    double length = 0.0;
    std::optional<std::string> kind;
    std::vector<NoteParamData> params;
};

/**
 * {
      "t": 4800,
      "e": "FocusCamera",
      "v": {
    "duration": 4,
    "x": 0,
    "y": 0,
    "char": 1,
    "ease": "CLASSIC"
   }
 * 全局唯一，整个铺面都用这一个事件，不区分难度。
 */
struct SongEventData {
    double time = 0.0;
    std::string eventKind;
    json value;
};

/**
 * 用于存储一首歌箭头数据的包装结构，里面自带了他所有的难度！您无需在外部在搞个映射
 * 每一个difficulty（作为值）应该和difficultyId（字符串）（作为键）unordered_map里面！
 * 不过不太理解的是，为何还要设计一个包装类包装一下chartData，直接字符串对应不是更清晰么？
 * 可能的原因：和json字段一块对应
* "easy": [
      {
        "t": 300,
        "d": 4,
        "l": 225,
        "p": [        ]
      },
      {
        "t": 600,
        "d": 5,
        "l": 0,
        "p": [        ]
      },
    ]
 * 就像这样，开头是一个easy数组。
 * 那么我们这个结构就是专门存这个的
 * 不过为什么我不直接用map呢，非得包装一下...
 * 算了就当简化代码
 */
struct SongDifficulty {
    /**
     * {难度ID, 铺面数据}
     * 什么！你说有些模组故意没有NoteData?
     * 那还玩个蛋，不玩了
     */
    std::unordered_map<string, std::vector<SongNoteData>> notes;

    // 默认构造函数用于默认拷贝（比如访问某个成员发生拷贝）
    SongDifficulty() = default;
};

/**
 * 什么叫ChartData，比如sky-chart.json，这一整个文件内容就是ChartData。
 * 每一个变体都有他自己的铺面文件
 * 他里面存储了完整的铺面数据（通过notes成员）和难度对应关系
 * 甭管原版是怎么设计的，我就这样设计的。
 */
struct SongChartData {
    std::string version;
    // 对应json的"easy""normal""hard"等字段
    SongDifficulty difficulties;
    // 对应event数组
    std::vector<SongEventData> events;                      // 事件数组
    std::unordered_map<std::string, double> scrollSpeed;              // 键值对 难度 : 滚动速度
    std::string generatedBy;
    std::string variation;
};

// NoteParamData 的 from_json
inline void from_json(const json& j, NoteParamData& p) {
    if (j.contains("n")) p.name = j["n"].get<std::string>();
    if (j.contains("v")) p.value = j["v"];
}

// SongNoteData 的 from_json
inline void from_json(const json& j, SongNoteData& note) {
    if (j.contains("t")) note.time = j["t"].get<double>();
    if (j.contains("d")) note.data = j["d"].get<int>();
    if (j.contains("l")) note.length = j["l"].get<double>();
    if (j.contains("k")) note.kind = j["k"].get<std::string>();
    if (j.contains("p") && j["p"].is_array()) {
        note.params = j["p"].get<std::vector<NoteParamData>>();
    }
}

// SongEventData 的 from_json
inline void from_json(const json& j, SongEventData& event) {
    if (j.contains("t")) event.time = j["t"].get<double>();
    if (j.contains("e")) event.eventKind = j["e"].get<std::string>();
    if (j.contains("v")) event.value = j["v"];
}

// SongChartData 的 from_json，调用这个函数即可一键完成解析！
inline void from_json(const json& j, SongChartData& chart) {
    // version
    if (j.contains("version")) {
        chart.version = j["version"].get<std::string>();
    }

    // generatedBy
    if (j.contains("generatedBy")) {
        chart.generatedBy = j["generatedBy"].get<std::string>();
    }

    // scrollSpeed: Map<String, Float>
    if (j.contains("scrollSpeed") && j["scrollSpeed"].is_object()) {
        for (auto& [key, val] : j["scrollSpeed"].items()) {
            chart.scrollSpeed[key] = val.get<double>();
        }
    }

    // events: Array<SongEventData>
    if (j.contains("events") && j["events"].is_array()) {
        chart.events = j["events"].get<std::vector<SongEventData>>();
    }

    // notes: Map<String, Array<SongNoteData>>
    // 我们也不用手动操作Difficulty结构了，直接一键解析完成了
    if (j.contains("notes") && j["notes"].is_object()) {
        for (auto& [difficulty, notesArray] : j["notes"].items()) {
            if (notesArray.is_array()) {
                chart.difficulties.notes[difficulty] = notesArray.get<std::vector<SongNoteData>>();
            }
        }
    }

    // variation 不在 JSON 中，由代码填充，用于存储到Song对象的difficulty里面
}

class SongDataParser {
    static SongTimeFormat SongTimeFormat_from_json_VS(const json& j);

    static SongTimeChange SongTimeChange_from_json_VS(const json& j) {
        SongTimeChange t;
        if (j.contains("t")) t.timeStamp = j["t"].get<double>();
        if (j.contains("b")) t.beatTime = j["b"].get<double>();
        if (j.contains("bpm")) t.bpm = j["bpm"].get<double>();
        if (j.contains("n")) t.timeSignatureNum = j["n"].get<int>();
        if (j.contains("d")) t.timeSignatureDen = j["d"].get<int>();
        if (j.contains("bt")) t.beatTuplets = j["bt"].get<std::vector<int>>();
        return t;
    }

    static SongOffsets SongOffsets_from_json_VS(const json& j) {
        SongOffsets o;
        if (j.contains("instrumental")) o.instrumental = j["instrumental"].get<double>();
        if (j.contains("altInstrumentals")) {
            for (auto& [key, val] : j["altInstrumentals"].items()) {
                o.altInstrumentals[key] = val.get<double>();
            }
        }
        if (j.contains("vocals")) {
            for (auto& [key, val] : j["vocals"].items()) {
                o.vocals[key] = val.get<double>();
            }
        }
        if (j.contains("altVocals")) {
            for (auto& [key, val] : j["altVocals"].items()) {
                for (auto& [charKey, charVal] : val.items()) {
                    o.altVocals[key][charKey] = charVal.get<double>();
                }
            }
        }
        return o;
    }

    static SongCharacterData SongCharacterData_from_json_VS(const json& j);

    static SongPlayData SongPlayData_from_json_VS(const json& j);

    // ==================== 2.0.x 解析和迁移 ====================
    static SongMetadata_v2_0_0 SongMetadata_v2_0_0_from_json(const json& j);

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

    // ==================== 2.1.x 解析和迁移 ====================
    static SongMetadata_v2_1_0 SongMetadata_v2_1_0_from_json(const json& j) {
        SongMetadata_v2_1_0 m;

        if (j.contains("version")) m.version = j["version"].get<std::string>();
        if (j.contains("songName")) m.songName = j["songName"].get<std::string>();
        if (j.contains("artist")) m.artist = j["artist"].get<std::string>();
        if (j.contains("charter")) m.charter = j["charter"].get<std::string>();
        if (j.contains("divisions")) m.divisions = j["divisions"].get<int>();
        if (j.contains("looped")) m.looped = j["looped"].get<bool>();
        if (j.contains("offsets")) m.offsets = SongOffsets_from_json_VS(j["offsets"]);
        if (j.contains("playData")) m.playData = SongPlayData_from_json_VS(j["playData"]);
        if (j.contains("generatedBy")) m.generatedBy = j["generatedBy"].get<std::string>();
        if (j.contains("timeFormat")) m.timeFormat = SongTimeFormat_from_json_VS(j["timeFormat"]);
        if (j.contains("timeChanges") && j["timeChanges"].is_array()) {
            for (const auto& item : j["timeChanges"]) {
                m.timeChanges.push_back(SongTimeChange_from_json_VS(item));
            }
        }

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

    // ==================== 2.2.x 直接解析 ====================
    static SongMetaData parseMetadata_v2_2_x(const json& j) {
        SongMetaData m;

        if (j.contains("version")) m.version = j["version"].get<std::string>();
        if (j.contains("songName")) m.songName = j["songName"].get<std::string>();
        if (j.contains("artist")) m.artist = j["artist"].get<std::string>();
        if (j.contains("charter")) m.charter = j["charter"].get<std::string>();
        if (j.contains("divisions")) m.divisions = j["divisions"].get<int>();
        if (j.contains("looped")) m.looped = j["looped"].get<bool>();
        if (j.contains("offsets")) m.offsets = SongOffsets_from_json_VS(j["offsets"]);
        if (j.contains("playData")) m.playData = SongPlayData_from_json_VS(j["playData"]);
        if (j.contains("generatedBy")) m.generatedBy = j["generatedBy"].get<std::string>();
        if (j.contains("timeFormat")) m.timeFormat = SongTimeFormat_from_json_VS(j["timeFormat"]);
        if (j.contains("timeChanges") && j["timeChanges"].is_array()) {
            for (const auto& item : j["timeChanges"]) {
                m.timeChanges.push_back(SongTimeChange_from_json_VS(item));
            }
        }

        return m;
    }

public:
    // 外部只需要调用这个就可以直接解析！（返回值是optional的都自带记录错误，因此你无需再log异常）
    static std::optional<SongMetaData> parseSongMetaData(const json& j, const std::string& filename = "") {
        semver::version current_version;
        std::string versionStr = "2.2.4";

        if (j.contains("version") && j["version"].is_string()) {
            versionStr = j["version"].get<std::string>();
        }

        auto parseResult = semver::parse(versionStr, current_version);
        if (!parseResult) {
            Exception::logVersionInvalid(versionStr, SongRegistryRelative::SONG_METADATA_VERSION_RULE, filename);
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
            auto old = SongMetadata_v2_1_0_from_json(j);
            return migrate_v2_1_to_v2_2(old);
        }
        else if (current_version >= v2_0_0 && current_version < v2_1_0) {
            // 2.0.x：解析旧结构 → 迁移
            auto old = SongMetadata_v2_0_0_from_json(j);
            return migrate_v2_0_to_v2_2(old);
        }
        else {
            // 不支持的版本
            Exception::logVersionInvalid(versionStr, SongRegistryRelative::SONG_METADATA_VERSION_RULE, filename);
            return std::nullopt;
        }
    }

    // 目前还没有看到和版本号有关的迁移内容，因此我推断他应该没有太大的变动，因此不提供迁移逻辑
    static SongChartData parseSongChartData(const json& j, const std::string& filename = "")
    {
        SongChartData s;
        from_json(j,s);
        return s;
    }
};