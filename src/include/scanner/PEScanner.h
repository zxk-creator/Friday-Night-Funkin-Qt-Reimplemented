#pragma once

/**
 * PEScanner — Psych Engine 模组扫描器
 *
 * 专门处理 PE 格式模组的解析。
 * 与 VS 格式完全独立，不做任何格式转换。
 */

#include <QString>
#include <QVector>
#include <vector>
#include <optional>
#include <memory>

#include "nlohmann/json.hpp"
#include "data/mod/ModMetadata.h"
#include "data/pe/PECharacterData.h"
#include "data/pe/PESongData.h"
#include "data/pe/PEStageData.h"

using json = nlohmann::json;

class PEModScanner
{
public:
    static std::unique_ptr<ModMetadata> scanMetadata(const QString& modPath);

    // 注意：unique_ptr 不可拷贝，必须使用 std::vector（而非 QVector）
    using CharacterPair = std::pair<QString, std::unique_ptr<PECharacterData>>;

    static std::vector<CharacterPair> scanCharacters(const QString& modPath);

    struct PESongScanResult {
        QString songId;
        QVector<PESongDifficultyData> difficulties;
    };
    static QVector<PESongScanResult> scanSongs(const QString& modPath);

    using WeekPair = std::pair<QString, PEWeekData>;
    static std::vector<WeekPair> scanWeeks(const QString& modPath);

    using StagePair = std::pair<QString, PEStageData>;
    static std::vector<StagePair> scanStages(const QString& modPath);

    static QStringList scanNoteStyleRefs(const QString& modPath);

private:
    static QStringList findFilesInMod(const QString& modPath, const QString& subDir, const QString& filter);
    static std::optional<PESongDifficultyData> parseOneSongFile(const QString& filePath);
    static QString inferDifficultyFromFilename(const QString& fileName, const QString& songName);
};
