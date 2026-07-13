#pragma once

#include <QString>
#include <QVector>
#include <QVariant>
#include <optional>
#include <memory>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

/**
 * Psych Engine 歌曲数据结构（独立于 VS）
 *
 * 对应 PE SwagSong 格式：
 * {
 *   "song": {
 *     "song": "Test",
 *     "notes": [...sections],
 *     "events": [[time, [[name, val1, val2]]]],
 *     "bpm": 100,
 *     "speed": 1.0,
 *     "player1": "bf",
 *     "player2": "dad",
 *     "gfVersion": "gf",
 *     "stage": "mainStage",
 *     "arrowSkin": "NOTE_assets",
 *     "splashSkin": "noteSplashes",
 *     ...
 *   }
 * }
 *
 * 解析后，PE 的每首歌可能有多个难度文件，
 * 每个文件都包含完整 SwagSong 结构。
 */

/**
 * PE 谱面段落
 */
struct PESection
{
    QVector<QVector<QVariant>> sectionNotes;  // [[strumTime, noteData, sustainLength, noteType], ...]
    float sectionBeats = 4.0f;
    bool mustHitSection = true;
    bool altAnim = false;
    bool gfSection = false;
    std::optional<float> bpm;
    bool changeBPM = false;

    void from_json(const json& j);
};

/**
 * PE 单首难度歌曲数据
 */
struct PESongFile
{
    // ======= 元数据 =======
    QString song;           // 歌曲显示名
    float bpm = 100.0f;
    float speed = 1.0f;
    float offset = 0.0f;
    bool needsVoices = true;

    // ======= 角色 =======
    QString player1 = "bf";
    QString player2 = "dad";
    QString gfVersion = "gf";
    QString stage = "mainStage";

    // ======= 谱面 =======
    QVector<PESection> notes;
    QVector<QVector<QVariant>> events;  // [[time, [[name, val1, val2]]]]

    // ======= 视觉 =======
    std::optional<QString> arrowSkin;
    std::optional<QString> splashSkin;
    bool disableNoteRGB = false;

    // ======= 失败 =======
    std::optional<QString> gameOverChar;
    std::optional<QString> gameOverSound;
    std::optional<QString> gameOverLoop;
    std::optional<QString> gameOverEnd;

    // ======= 版本 =======
    QString format = "psych_v1";

    void from_json(const json& j);
};

/**
 * PE 难度文件解析结果（一首歌一个难度的完整数据）
 */
struct PESongDifficultyData
{
    QString difficulty;         // 难度名（从文件名推断）
    PESongFile songData;        // 歌曲数据
    QVector<json> rawEvents;    // 从 events.json 补充的事件
};

/**
 * PE 周目文件
 * 对应 WeekFile 格式
 */
struct PEWeekData
{
    QVector<QVector<QVariant>> songs;  // [[songName, iconName, [R,G,B]], ...]
    QVector<QString> weekCharacters;   // [opponent, boyfriend, girlfriend]
    QString weekBackground;
    QString weekBefore;
    QString storyName;
    QString weekName;
    bool startUnlocked = true;
    bool hiddenUntilUnlocked = false;
    bool hideStoryMode = false;
    bool hideFreeplay = false;
    QString difficulties;

    void from_json(const json& j);
};
