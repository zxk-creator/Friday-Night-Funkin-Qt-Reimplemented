#include "scanner/PEScanner.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

#include "nlohmann/json.hpp"
#include "utils/file/FileUtil.h"
#include "utils/Path.h"
#include "utils/message/MessageHandler.h"
#include "utils/exception/CustomException.h"
// PEScanner 只负责解析，不负责注册
// 注册由 ModScanner 调用方处理

// ==================== 元数据扫描 ====================

std::unique_ptr<ModMetadata> PEModScanner::scanMetadata(const QString& modPath)
{
    auto meta = std::make_unique<ModMetadata>();
    meta->modPath = modPath;
    meta->engineType = ModEngineType::PE;

    auto id = FileUtil::getPathLeaf(modPath);
    meta->id = id.value_or("Unnamed");

    QDir modDir(modPath);
    QString cfgPath = modDir.filePath("pack.json");
    if (!QFile::exists(cfgPath)) {
        LOG_WARNING(false, modPath + "没有pack.json！");
        return meta;
    }

    QFile cfgFile(cfgPath);
    if (!cfgFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_WARNING(false, "无法打开" + cfgPath);
        return meta;
    }

    try {
        auto j = json::parse(cfgFile.readAll().toStdString(), nullptr, true, true);

        meta->title = QString::fromStdString(j.value("name", "未命名模组"));
        meta->description = QString::fromStdString(j.value("description", "无描述"));
        meta->restart = j.value("restart", false);
        meta->runsGlobally = j.value("runsGlobally", false);

        if (j.contains("color") && j["color"].is_array() && j["color"].size() >= 3)
            meta->bgRGBColor = {j["color"][0].get<int>(), j["color"][1].get<int>(), j["color"][2].get<int>()};

        // 图标
        QString normalIcon = modDir.filePath("pack.png");
        QString pixelIcon = modDir.filePath("pack-pixel.png");
        if (QFileInfo::exists(normalIcon))
            meta->iconPath = normalIcon;
        else if (QFileInfo::exists(pixelIcon))
            meta->iconPath = pixelIcon;

        LOG_INFO("成功解析 PE 模组元数据: " + meta->id.value_or("?"));
    } catch (const std::exception& e) {
        LOG_ERROR(true, QString("解析PE pack.json异常: ") + e.what());
    }

    return meta;
}

// ==================== 角色扫描 ====================

std::vector<PEModScanner::CharacterPair>
PEModScanner::scanCharacters(const QString& modPath)
{
    std::vector<CharacterPair> results;

    auto files = findFilesInMod(modPath, "characters", "*.json");
    for (const auto& file : files) {
        try {
            QString raw = FileUtil::ReadFileToString(file);
            auto j = json::parse(raw.toStdString());

            auto cd = std::make_unique<PECharacterData>();
            cd->from_json(j);
            cd->id = FileUtil::fetchIdFromFileName(file);

            LOG_INFO("成功解析 PE 角色: " + cd->id);
            results.push_back({cd->id, std::move(cd)});
        } catch (const std::exception& e) {
            LOG_JSON_PARSE_ERROR(true, e.what(), file);
        }
    }

    return results;
}

// ==================== 歌曲扫描 ====================

QVector<PEModScanner::PESongScanResult>
PEModScanner::scanSongs(const QString& modPath)
{
    QVector<PESongScanResult> results;

    // PE 歌曲目录可能在 modPath/data/ 下
    QStringList searchPaths = {
        QDir::cleanPath(modPath + "/data"),
        QDir::cleanPath(modPath + "/songs")
    };

    for (const auto& dataPath : searchPaths) {
        QDir dir(dataPath);
        if (!dir.exists()) continue;

        QStringList songDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const auto& songDir : songDirs) {
            PESongScanResult result;
            result.songId = songDir;

            QString fullPath = QDir::cleanPath(dataPath + "/" + songDir);
            auto jsonFiles = FileUtil::getFileAbsolutePaths(fullPath, "*.json");
            if (!jsonFiles) continue;

            // 遍历难度文件
            for (const auto& jsonFile : jsonFiles.value()) {
                QString fileName = QFileInfo(jsonFile).fileName();

                // 跳过 events.json
                if (fileName == "events.json") continue;

                auto diffData = parseOneSongFile(jsonFile);
                if (diffData) {
                    // 如果没从文件名推断出难度名，标记为 "normal"
                    if (diffData->difficulty.isEmpty())
                        diffData->difficulty = "normal";
                    result.difficulties.push_back(std::move(*diffData));
                }
            }

            if (!result.difficulties.isEmpty()) {
                LOG_INFO("成功解析 PE 歌曲: " + result.songId
                         + " (" + QString::number(result.difficulties.size()) + "个难度)");
                results.push_back(std::move(result));
            }
        }
    }

    return results;
}

std::optional<PESongDifficultyData>
PEModScanner::parseOneSongFile(const QString& filePath)
{
    try {
        QString raw = FileUtil::ReadFileToString(filePath);
        json j = json::parse(raw.toStdString());

        PESongDifficultyData result;
        result.songData.from_json(j);
        result.difficulty = inferDifficultyFromFilename(
            QFileInfo(filePath).fileName(),
            result.songData.song);

        return result;
    } catch (const std::exception& e) {
        LOG_JSON_PARSE_ERROR(true, e.what(), filePath);
        return std::nullopt;
    }
}

QString PEModScanner::inferDifficultyFromFilename(const QString& fileName, const QString& songName)
{
    // PE 命名规则: songName-difficulty.json
    // 或者 songName.json (此时默认为 hard)
    QString base = QFileInfo(fileName).completeBaseName();

    // 去掉歌曲名前缀
    if (base.startsWith(songName, Qt::CaseInsensitive)) {
        QString diff = base.mid(songName.length());
        // 去掉开头的 - 或 _
        while (diff.startsWith('-') || diff.startsWith('_') || diff.startsWith(' '))
            diff = diff.mid(1);
        if (!diff.isEmpty()) return diff.toLower();
    }

    // 如果歌曲名 != 基础名，检查是否有已知难度后缀
    QString lower = base.toLower();
    if (lower.endsWith("-easy") || lower.endsWith("_easy")) return "easy";
    if (lower.endsWith("-normal") || lower.endsWith("_normal")) return "normal";
    if (lower.endsWith("-hard") || lower.endsWith("_hard")) return "hard";
    if (lower.endsWith("-erect") || lower.endsWith("_erect")) return "erect";
    if (lower.endsWith("-nightmare") || lower.endsWith("_nightmare")) return "nightmare";

    return "hard"; // PE 默认
}

// ==================== 周目扫描 ====================

std::vector<PEModScanner::WeekPair>
PEModScanner::scanWeeks(const QString& modPath)
{
    std::vector<WeekPair> results;

    auto files = findFilesInMod(modPath, "weeks", "*.json");
    for (const auto& file : files) {
        try {
            QString raw = FileUtil::ReadFileToString(file);
            json j = json::parse(raw.toStdString());

            PEWeekData week;
            week.from_json(j);

            QString id = FileUtil::fetchIdFromFileName(file);
            LOG_INFO("成功解析 PE 周目: " + id);
            results.push_back({id, std::move(week)});
        } catch (const std::exception& e) {
            LOG_JSON_PARSE_ERROR(true, e.what(), file);
        }
    }

    return results;
}

// ==================== 舞台扫描 ====================

std::vector<PEModScanner::StagePair>
PEModScanner::scanStages(const QString& modPath)
{
    std::vector<StagePair> results;

    auto files = findFilesInMod(modPath, "stages", "*.json");
    for (const auto& file : files) {
        try {
            QString raw = FileUtil::ReadFileToString(file);
            json j = json::parse(raw.toStdString());

            PEStageData stage;
            stage.from_json(j);

            QString id = FileUtil::fetchIdFromFileName(file);
            LOG_INFO("成功解析 PE 舞台: " + id);
            results.push_back({id, std::move(stage)});
        } catch (const std::exception& e) {
            LOG_JSON_PARSE_ERROR(true, e.what(), file);
        }
    }

    return results;
}

// ==================== 箭头皮肤引用收集 ====================

QStringList PEModScanner::scanNoteStyleRefs(const QString& modPath)
{
    QStringList refs;

    // PE 中箭头皮肤引用在歌曲 JSON 的 arrowSkin / splashSkin 字段中
    auto songResults = scanSongs(modPath);
    for (const auto& song : songResults) {
        for (const auto& diff : song.difficulties) {
            if (diff.songData.arrowSkin && !refs.contains(*diff.songData.arrowSkin))
                refs.push_back(*diff.songData.arrowSkin);
            if (diff.songData.splashSkin && !refs.contains(*diff.songData.splashSkin))
                refs.push_back(*diff.songData.splashSkin);
        }
    }

    return refs;
}

// ==================== 辅助 ====================

QStringList PEModScanner::findFilesInMod(const QString& modPath, const QString& subDir, const QString& filter)
{
    QStringList results;

    // PE 模组结构可能有多种组织形式：
    // modPath/subDir/
    // modPath/assets/shared/subDir/  (shared 目录)
    QStringList searchPaths = {
        QDir::cleanPath(modPath + "/" + subDir),
        QDir::cleanPath(modPath + "/assets/shared/" + subDir),
    };

    for (const auto& path : searchPaths) {
        auto files = FileUtil::getFileAbsolutePaths(path, filter);
        if (files) results.append(files.value());
    }

    return results;
}
