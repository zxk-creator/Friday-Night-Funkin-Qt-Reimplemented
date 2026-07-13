#pragma once

/**
 * VSScanner — V-Slice 模组内容扫描器
 *
 * 专门处理 V-Slice 格式模组的解析。
 * 解析结果以 VS 原生类型返回，不做格式转换。
 */

#include <QString>
#include <QVector>
#include <vector>
#include <optional>
#include <memory>

#include "nlohmann/json.hpp"
#include "data/mod/ModMetadata.h"
#include "data/character/CharacterData.h"
#include "data/level/LevelData.h"
#include "data/stage/StageData.h"
#include "data/notestyle/NoteStyleData.h"
#include "play/song/Song.h"

using json = nlohmann::json;

class VSModScanner
{
public:
    static std::unique_ptr<ModMetadata> scanMetadata(const QString& modPath);

    // 注意：unique_ptr 不可拷贝，必须使用 std::vector（而非 QVector）
    using CharacterPair = std::pair<QString, std::unique_ptr<CharacterData>>;
    using SongPair = std::pair<QString, std::unique_ptr<Song>>;
    using LevelPair = std::pair<QString, std::unique_ptr<LevelData>>;
    using StagePair = std::pair<QString, std::unique_ptr<StageData>>;
    using NoteStylePair = std::pair<QString, std::unique_ptr<NoteStyleData>>;

    static std::vector<CharacterPair> scanCharacters(const QString& modPath);
    static std::vector<SongPair> scanSongs(const QString& modPath);
    static std::vector<LevelPair> scanLevels(const QString& modPath);
    static std::vector<StagePair> scanStages(const QString& modPath);
    static std::vector<NoteStylePair> scanNoteStyles(const QString& modPath);

private:
    static QString getDataPath(const QString& modPath, const QString& subDir);
    static std::unique_ptr<Song> parseOneSong(const QString& modPath, const QString& songId);
};
