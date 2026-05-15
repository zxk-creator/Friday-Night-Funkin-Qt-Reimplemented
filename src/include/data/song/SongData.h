//
// Created by kkplay on 4/25/26.
//

#pragma once

#include "nlohmann/json.hpp"
#include <optional>
#include <QVector>
#include <memory>
#include <iostream>

#include "Constants.h"
#include "play/ISerializable.h"
#include "semver/semver.h"
#include "utils/exception/CustomException.h"

using json = nlohmann::json;

enum class SongTimeFormat {
    TICKS,
    FLOAT,
    MILLISECONDS
};

/**
 * 用于定义歌曲中 BPM 变化点 以及对应的 拍号信息。
 */
struct SongTimeChange : ISerializable {
    float timeStamp = 0.0;
    float beatTime = 0.0;
    float bpm = 100.0;
    int timeSignatureNum = 4;
    int timeSignatureDen = 4;
    QVector<int> beatTuplets = {4, 4, 4, 4};

    SongTimeChange() = default;
    SongTimeChange(float t, float bpm) : timeStamp(t), bpm(bpm) {}

    QString toString() const override
    {
        QString res = "时间戳: " + QString::number(timeStamp)
                       + ", BPM: " + QString::number(bpm)
                       + ", 拍号: " + QString::number(timeSignatureNum) + "/" + QString::number(timeSignatureDen);

        if (!beatTuplets.isEmpty()) {
            res += ", 连音: [";
            for (int i = 0; i < beatTuplets.size(); ++i) {
                res += QString::number(beatTuplets[i]);
                if (i < beatTuplets.size() - 1) res += ", ";
            }
            res += "]";
        }
        return res;
    }

    QString oneToString(const QString& id) const override
    {
        return "";
    }

    // nlohmann 自动转换
    void from_json(const json& j) {
        if (j.contains("t")) timeStamp = j["t"].get<float>();
        if (j.contains("b")) beatTime = j["b"].get<float>();
        if (j.contains("bpm")) bpm = j["bpm"].get<float>();
        if (j.contains("n")) timeSignatureNum = j["n"].get<int>();
        if (j.contains("d")) timeSignatureDen = j["d"].get<int>();
        if (j.contains("bt")) {
            auto btArray = j["bt"].get<std::vector<int>>();
            beatTuplets = QVector<int>(btArray.begin(), btArray.end());
        }
    }
};

/**
 * 用于微调音频与谱面的同步，解决由于音频编码延迟、剪辑问题或硬件差异导致的音画不同步。
 */
struct SongOffsets : ISerializable {
    float instrumental = 0.0;

    /**
     * String: 角色id或变体id。
     */
    QHash<QString, float> altInstrumentals;
    QHash<QString, float> vocals;
    QHash<QString, QHash<QString, float>> altVocals;

    float getInstrumentalOffset(const QString& instrumentalId = "") const {
        if (instrumentalId.isEmpty()) return instrumental;
        auto it = altInstrumentals.find(instrumentalId);
        return (it != altInstrumentals.end()) ? it.value() : instrumental;
    }

    float getVocalOffset(const QString& charId, const QString& instrumentalId = "") const {
        if (!instrumentalId.isEmpty()) {
            auto itInst = altVocals.find(instrumentalId);
            if (itInst != altVocals.end()) {
                auto itChar = itInst.value().find(charId);
                if (itChar != itInst.value().end()) return itChar.value();
            }
        }
        auto it = vocals.find(charId);
        return (it != vocals.end()) ? it.value() : 0.0;
    }

    QString toString() const override
    {
        QString res = "乐器偏移: " + QString::number(instrumental) + "\n";

        if (!altInstrumentals.isEmpty()) {
            res += "替代乐器偏移: \n";
            for (auto it = altInstrumentals.constBegin(); it != altInstrumentals.constEnd(); ++it) {
                res += "  " + it.key() + ": " + QString::number(it.value()) + "\n";
            }
        }

        if (!vocals.isEmpty()) {
            res += "人声偏移: \n";
            for (auto it = vocals.constBegin(); it != vocals.constEnd(); ++it) {
                res += "  " + it.key() + ": " + QString::number(it.value()) + "\n";
            }
        }

        if (!altVocals.isEmpty()) {
            res += "替代人声偏移: \n";
            for (auto it = altVocals.constBegin(); it != altVocals.constEnd(); ++it) {
                res += "  " + it.key() + ":\n";
                const auto& innerHash = it.value();
                for (auto innerIt = innerHash.constBegin(); innerIt != innerHash.constEnd(); ++innerIt) {
                    res += "    " + innerIt.key() + ": " + QString::number(innerIt.value()) + "\n";
                }
            }
        }

        return res;
    }

    QString oneToString(const QString& id) const override
    {
        return "";
    }

    // nlohmann 自动转换
    void from_json(const json& j) {
        if (j.contains("instrumental")) instrumental = j["instrumental"].get<float>();
        if (j.contains("altInstrumentals")) {
            for (auto& [key, val] : j["altInstrumentals"].items()) {
                altInstrumentals[QString::fromStdString(key)] = val.get<float>();
            }
        }
        if (j.contains("vocals")) {
            for (auto& [key, val] : j["vocals"].items()) {
                vocals[QString::fromStdString(key)] = val.get<float>();
            }
        }
        if (j.contains("altVocals")) {
            for (auto& [key, val] : j["altVocals"].items()) {
                QString instKey = QString::fromStdString(key);
                for (auto& [charKey, charVal] : val.items()) {
                    altVocals[instKey][QString::fromStdString(charKey)] = charVal.get<float>();
                }
            }
        }
    }
};

struct SongCharacterData : ISerializable {
    QString player = "bf";
    QString girlfriend = "gf";
    QString opponent = "dad";
    QString instrumental = "";
    // 所谓"替代伴奏"到底是什么意思
    // 我认为特可能是为erect什么准备的，不知道
    // 存储的是歌曲的绝对路径！播放就从这里拿路径
    std::vector<QString> altInstrumentals;
    // 据说这个数组里面的音效会同时播放
    std::vector<QString> opponentVocals;
    // 也会同时播放
    std::vector<QString> playerVocals;

    SongCharacterData() = default;
    SongCharacterData(const QString& p, const QString& gf, const QString& opp)
        : player(p), girlfriend(gf), opponent(opp) {}

    QString toString() const override
    {
        QString division = "--- 角色信息 ---\n";
        QString res;
        res += division + "右边角色: " + player + "\n";
        res += "左边角色: " + opponent + "\n";
        res += "中间角色: " + girlfriend + "\n";
        res += "伴奏: " + instrumental + "\n";

        res += "替代伴奏: ";
        for (int i = 0; i < altInstrumentals.size(); ++i) {
            res += altInstrumentals[i];
            if (i < altInstrumentals.size() - 1) res += ", ";
        }
        res += "\n";

        res += "玩家音效: ";
        for (int i = 0; i < playerVocals.size(); ++i) {
            res += playerVocals[i];
            if (i < playerVocals.size() - 1) res += ", ";
        }
        res += "\n";

        res += "对手音效: ";
        for (int i = 0; i < opponentVocals.size(); ++i) {
            res += opponentVocals[i];
            if (i < opponentVocals.size() - 1) res += ", ";
        }
        res += "\n";

        return res;
    }

    QString oneToString(const QString& id) const override
    {
        return "";
    }

    // nlohmann 自动转换
    void from_json(const json& j) {
        if (j.contains("player")) player = QString::fromStdString(j["player"].get<std::string>());
        if (j.contains("girlfriend")) girlfriend = QString::fromStdString(j["girlfriend"].get<std::string>());
        if (j.contains("opponent")) opponent = QString::fromStdString(j["opponent"].get<std::string>());
        if (j.contains("instrumental")) instrumental = QString::fromStdString(j["instrumental"].get<std::string>());

        if (j.contains("altInstrumentals")) {
            altInstrumentals.clear();
            for (const auto& item : j["altInstrumentals"]) {
                altInstrumentals.push_back(QString::fromStdString(item.get<std::string>()));
            }
        }
        if (j.contains("opponentVocals")) {
            opponentVocals.clear();
            for (const auto& item : j["opponentVocals"]) {
                opponentVocals.push_back(QString::fromStdString(item.get<std::string>()));
            }
        }
        if (j.contains("playerVocals")) {
            playerVocals.clear();
            for (const auto& item : j["playerVocals"]) {
                playerVocals.push_back(QString::fromStdString(item.get<std::string>()));
            }
        }
    }
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
 * 就叫playData，包含了难度数据（只有字符串），歌曲角色等
 */
struct SongPlayData : ISerializable {
    // 传说中的变体，-erect，-pico等
    QVector<QString> songVariations;
    QVector<QString> difficulties;
    SongCharacterData characters;
    QString stage = "mainStage";
    QString noteStyle = "funkin";
    QHash<QString, int> ratings;
    std::optional<QString> album;
    std::optional<QString> stickerPack;
    int previewStart = 0;
    int previewEnd = 15000;

    QString toString() const override
    {
        QString res;

        if (!difficulties.isEmpty()) {
            res += "可用难度: ";
            for (int i = 0; i < difficulties.size(); ++i) {
                res += difficulties[i];
                if (i < difficulties.size() - 1) res += " ";
            }
            res += "\n";
        }

        if (!songVariations.isEmpty()) {
            res += "变体: ";
            for (int i = 0; i < songVariations.size(); ++i) {
                res += songVariations[i];
                if (i < songVariations.size() - 1) res += " ";
            }
            res += "\n";
        }

        res += "舞台: " + stage + "\n";
        res += "音符风格: " + noteStyle + "\n";
        res += "预览范围: " + QString::number(previewStart) + " - " + QString::number(previewEnd) + "\n";

        if (!ratings.isEmpty()) {
            res += "难度评级: \n";
            for (auto it = ratings.constBegin(); it != ratings.constEnd(); ++it) {
                res += "  " + it.key() + ": " + QString::number(it.value()) + "\n";
            }
        }

        if (album.has_value()) {
            res += "专辑: " + album.value() + "\n";
        }
        if (stickerPack.has_value()) {
            res += "贴纸包: " + stickerPack.value() + "\n";
        }

        res += characters.toString();

        return res;
    }

    QString oneToString(const QString& id) const override
    {
        return "";
    }

    // nlohmann 自动转换
    void from_json(const json& j) {
        if (j.contains("songVariations")) {
            songVariations.clear();
            for (const auto& item : j["songVariations"]) {
                songVariations.push_back(QString::fromStdString(item.get<std::string>()));
            }
        }
        if (j.contains("difficulties")) {
            difficulties.clear();
            for (const auto& item : j["difficulties"]) {
                difficulties.push_back(QString::fromStdString(item.get<std::string>()));
            }
        }
        if (j.contains("characters")) {
            characters.from_json(j["characters"]);
        }
        if (j.contains("stage")) stage = QString::fromStdString(j["stage"].get<std::string>());
        if (j.contains("noteStyle")) noteStyle = QString::fromStdString(j["noteStyle"].get<std::string>());
        if (j.contains("ratings")) {
            for (auto& [key, val] : j["ratings"].items()) {
                ratings[QString::fromStdString(key)] = val.get<int>();
            }
        }
        if (j.contains("album")) album = QString::fromStdString(j["album"].get<std::string>());
        if (j.contains("stickerPack")) stickerPack = QString::fromStdString(j["stickerPack"].get<std::string>());
        if (j.contains("previewStart")) previewStart = j["previewStart"].get<int>();
        if (j.contains("previewEnd")) previewEnd = j["previewEnd"].get<int>();
    }
};

/**
 * 包含PlayData，CharacterData（定义对手，玩家，女朋友是谁），Offsets等
 * 请注意，这里面并不存储任何绝对路径，他只存ID，路径都是运行时拼接出来的
 */
struct SongMetaData : ISerializable {
    QString version = "2.2.4";
    QString songName = "Unknown";
    QString artist = "Unknown";
    std::optional<QString> charter;
    std::optional<int> divisions = 96;
    bool looped = false;
    SongOffsets offsets;
    SongPlayData playData;
    QString generatedBy;
    SongTimeFormat timeFormat = SongTimeFormat::MILLISECONDS;
    QVector<SongTimeChange> timeChanges;

    SongMetaData() = default;

    QString toString() const override
    {
        QString division = "========== 歌曲元数据 ==========\n";
        QString res = division;

        // 基本信息
        res += "版本: " + version + "\n";
        res += "歌曲名: " + songName + "\n";
        res += "艺术家: " + artist + "\n";

        if (charter.has_value()) {
            res += "谱师: " + charter.value() + "\n";
        }
        if (divisions.has_value()) {
            res += "分割数: " + QString::number(divisions.value()) + "\n";
        }

        res += "是否循环: " + QString(looped ? "是" : "否") + "\n";
        res += "时间格式: " + QString(timeFormat == SongTimeFormat::MILLISECONDS ? "毫秒" : "节拍") + "\n";
        res += "生成工具: " + generatedBy + "\n";

        res += "\n--- 偏移量 ---\n";
        res += offsets.toString();

        res += "\n--- 游玩数据 ---\n";
        res += playData.toString();

        if (!timeChanges.isEmpty()) {
            res += "\n--- 时间变化 ---\n";
            for (const auto& tc : timeChanges) {
                res += tc.toString() + "\n";
            }
        }

        return res + "\n" + division;
    }

    QString oneToString(const QString& id) const override
    {
        return "";
    }

    // nlohmann 自动转换
    void from_json(const json& j) {
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
};

// ==================== 2.0.x 临时结构体 ====================
struct SongMetadata_v2_0_0 {
    QString version;
    QString songName;
    QString artist;
    std::optional<QString> charter;
    std::optional<int> divisions = 96;
    bool looped = false;
    SongOffsets offsets;

    // 2.0.x 特有：字段在顶层
    QVector<QString> difficulties;
    SongCharacterData characters;
    QString stage = "mainStage";
    QString noteStyle = "funkin";
    QHash<QString, int> ratings;

    QString generatedBy;
    SongTimeFormat timeFormat = SongTimeFormat::MILLISECONDS;
    QVector<SongTimeChange> timeChanges;

    // nlohmann 自动转换
    void from_json(const json& j) {
        if (j.contains("version")) version = QString::fromStdString(j["version"].get<std::string>());
        if (j.contains("songName")) songName = QString::fromStdString(j["songName"].get<std::string>());
        if (j.contains("artist")) artist = QString::fromStdString(j["artist"].get<std::string>());
        if (j.contains("charter")) charter = QString::fromStdString(j["charter"].get<std::string>());
        if (j.contains("divisions")) divisions = j["divisions"].get<int>();
        if (j.contains("looped")) looped = j["looped"].get<bool>();
        if (j.contains("offsets")) offsets.from_json(j["offsets"]);

        // 2.0.x 特有：字段在顶层
        if (j.contains("difficulties")) {
            difficulties.clear();
            for (const auto& item : j["difficulties"]) {
                difficulties.push_back(QString::fromStdString(item.get<std::string>()));
            }
        }
        if (j.contains("characters")) {
            characters.from_json(j["characters"]);
        }
        if (j.contains("stage")) stage = QString::fromStdString(j["stage"].get<std::string>());
        if (j.contains("noteStyle")) noteStyle = QString::fromStdString(j["noteStyle"].get<std::string>());
        if (j.contains("ratings")) {
            for (auto& [key, val] : j["ratings"].items()) {
                ratings[QString::fromStdString(key)] = val.get<int>();
            }
        }

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
};

// ==================== 2.1.x 临时结构体 ====================
struct SongMetadata_v2_1_0 {
    QString version;
    QString songName;
    QString artist;
    std::optional<QString> charter;
    std::optional<int> divisions = 96;
    bool looped = false;
    SongOffsets offsets;
    SongPlayData playData;  // 2.1.x 已有 playData，但可能缺少 album/stickerPack
    QString generatedBy;
    SongTimeFormat timeFormat = SongTimeFormat::MILLISECONDS;
    QVector<SongTimeChange> timeChanges;

    // nlohmann 自动转换
    void from_json(const json& j);
};

struct NoteParamData : ISerializable {
    QString name;
    // value: Dynamic
    json value;

    QString toString() const override
    {
        return "箭头参数: \n名字: " + name + "\n" + "参数: \n" + QString::fromStdString(value.dump()) + "\n";
    }

    QString oneToString(const QString& id) const override
    {
        return "";
    }

    // nlohmann 自动转换
    void from_json(const json& j) {
        if (j.contains("n")) name = QString::fromStdString(j["n"].get<std::string>());
        if (j.contains("v")) value = j["v"];
    }
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
    // 不让他可打印吧，直接打印数字
    float time = 0.0;
    int data = 0;
    float length = 0.0;
    std::optional<QString> kind;
    QVector<NoteParamData> params;

    // nlohmann 自动转换
    void from_json(const json& j) {
        if (j.contains("t")) time = j["t"].get<float>();
        if (j.contains("d")) data = j["d"].get<int>();
        if (j.contains("l")) length = j["l"].get<float>();
        if (j.contains("k")) kind = QString::fromStdString(j["k"].get<std::string>());
        if (j.contains("p") && j["p"].is_array()) {
            params.clear();
            for (const auto& item : j["p"]) {
                NoteParamData p;
                p.from_json(item);
                params.push_back(p);
            }
        }
    }
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
    float time = 0.0;
    QString eventKind;
    json value;

    // nlohmann 自动转换
    void from_json(const json& j) {
        if (j.contains("t")) time = j["t"].get<float>();
        if (j.contains("e")) eventKind = QString::fromStdString(j["e"].get<std::string>());
        if (j.contains("v")) value = j["v"];
    }
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
struct SongDifficulty : ISerializable {
    /**
     * {难度ID, 铺面数据}
     * 什么！你说有些模组故意没有NoteData?
     * 那还玩个蛋，不玩了
     */
    QHash<QString, QVector<SongNoteData>> notes;

    // 默认构造函数用于默认拷贝（比如访问某个成员发生拷贝）
    SongDifficulty() = default;

    QString toString() const override
    {
        QHashIterator<QString, QVector<SongNoteData>> it(notes);

        QString res = "难度数据: \n";
        while (it.hasNext())
        {
            it.next();
            // eg: "easy: 90个note"
            res += it.key() + ": " + QString::number(it.value().size()) + "个note\n";
        }

        return res;
    }

    QString oneToString(const QString& id) const override
    {
        return "";
    }

    // 为什么没有from_json，因为他在SongChartData里面被自动处理了
};

/**
 * 什么叫ChartData，比如sky-chart.json，这一整个文件内容就是ChartData。
 * 每一个变体都有他自己的铺面文件
 * 他里面存储了完整的铺面数据（通过notes成员）和难度对应关系
 * 甭管原版是怎么设计的，我就这样设计的。
 */
struct SongChartData : ISerializable {
    QString version;
    // 对应json的"easy""normal""hard"等字段
    SongDifficulty difficulties;
    // 对应event数组
    QVector<SongEventData> events;
    // 难度 : 滚动速度
    QHash<QString, float> scrollSpeed;
    QString generatedBy;

    // 展现整个铺面内容
    QString toString() const override
    {
        QString division = "========== 铺面信息 ==========\n";
        // 我不信您能写21亿个事件
        int eventCount = events.size();
        QString _scrollSpeeds = "箭头滚动速度: \n";
        for (auto it = scrollSpeed.constBegin(); it != scrollSpeed.constEnd(); ++it)
        {
            // eg: easy: 1.6
            _scrollSpeeds += it.key() + ": " + QString::number(it.value()) + "\n";
        }

        return division + "事件数量: " + QString::number(eventCount) + "\n版本: " + version
                          + "\n铺面由: " + generatedBy + " 生成" + "\n" + _scrollSpeeds
                          + difficulties.toString() + "\n" + division + "\n\n";
    }

    QString oneToString(const QString& id) const override
    {
        return "";
    }

    void from_json(const json& j);
};

/**
 * SongDataParser - 负责解析和版本迁移
 * 解析逻辑全部移动到 .cpp 文件中
 * 目前还没有看到和版本号有关的迁移内容，因此我推断他应该没有太大的变动，因此不提供迁移逻辑
 */
class SongDataParser {
public:
    // 外部只需要调用这个就可以直接解析！（返回值是optional的都自带记录错误，因此你无需再log异常）
    static std::optional<SongMetaData> parseSongMetaData_VS(const json& j, const QString& filename = "");

    /**
     * 解析PE的元数据，您应该每首歌仅调用一次这个函数获得正确的metadata。不应该反复调用。
     * @param difficulties 根据铺面文件，所有的难度字符串
     * @param j 歌曲json内容（PE的铺面和元数据在一起）
     * @param filename 调试用：文件名
     * @param difficulty 由于难度与文件名绑定，需要手动传入难度数据。格式{easy,normal,hard,custom}
     * @return 官方引擎元数据格式
     */
    static SongMetaData parseSongMetaData_PE(const QVector<QString>& difficulties,const json& j,const QString& filename = "");

    static SongChartData parseSongChartData_VS(const json& j, const QString& filename = "");

    /**
     * 解析PE的铺面
     * @param j 歌曲json内容（PE的铺面和元数据在一起）
     * @param jEvents 单独的events.json文件内容
     * @param filename 调试用：文件名
     * @param difficulty 由于PE的难度是写在文件名里面的，而VS的难度是写在铺面里面的，所以解析PE时需手动传入难度。
     * @return 官方引擎格式铺面
     */
    static SongChartData parseSongChartData_PE(const QString& difficulty,const json& j,const QString& filename = "");

    /**
     * 解析PE单独的events.json 文件
     * @param j 内容
     * @param filename 文件名
     * @return QVector<SongEventData>事件列表
     */
    static QVector<SongEventData> parseEvents_PE(const json& j,const QString& filename = "");

    static SongChartData mergeChartData_PE(const QVector<SongChartData>& chartDatas,const QVector<SongEventData>& eventDatas);
};