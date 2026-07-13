#include "scanner/VSScanner.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

#include "nlohmann/json.hpp"
#include "utils/file/FileUtil.h"
#include "utils/Path.h"
#include "utils/message/MessageHandler.h"
#include "utils/exception/CustomException.h"
#include "data/song/SongData.h"

// VSScanner 只负责解析，不负责注册
// 注册由 ModScanner 调用方处理

// ==================== 元数据扫描 ====================

std::unique_ptr<ModMetadata> VSModScanner::scanMetadata(const QString& modPath)
{
    auto meta = std::make_unique<ModMetadata>();
    meta->modPath = modPath;
    meta->engineType = ModEngineType::VS;

    auto id = FileUtil::getPathLeaf(modPath);
    meta->id = id.value_or("Unnamed");

    QDir modDir(modPath);
    QString cfgPath = modDir.filePath("_polymod_meta.json");
    if (!QFile::exists(cfgPath)) {
        LOG_WARNING(false, modPath + "没有_polymod_meta.json！");
        return meta;
    }

    QFile cfgFile(cfgPath);
    if (!cfgFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_WARNING(false, "无法打开" + cfgPath);
        return meta;
    }

    try {
        auto j = json::parse(cfgFile.readAll().toStdString(), nullptr, true, true);

        meta->title = QString::fromStdString(j.value("title", "未命名模组"));
        meta->description = QString::fromStdString(j.value("description", "无描述"));

        if (j.contains("homepage") && j["homepage"].is_string())
            meta->homepage = QString::fromStdString(j["homepage"].get<std::string>());
        if (j.contains("api_version") && j["api_version"].is_string())
            meta->apiVersion = QString::fromStdString(j["api_version"].get<std::string>());
        if (j.contains("mod_version") && j["mod_version"].is_string())
            meta->modVersion = QString::fromStdString(j["mod_version"].get<std::string>());
        if (j.contains("license") && j["license"].is_string())
            meta->license = QString::fromStdString(j["license"].get<std::string>());

        QString iconPath = modDir.filePath("_polymod_icon.png");
        if (QFileInfo::exists(iconPath)) meta->iconPath = iconPath;

        if (j.contains("dependencies") && j["dependencies"].is_object()) {
            ModDependencies deps;
            for (auto& [key, val] : j["dependencies"].items())
                deps.insert(QString::fromStdString(key), QString::fromStdString(val.get<std::string>()));
            meta->dependencies = deps;
        }

        if (j.contains("optionalDependencies") && j["optionalDependencies"].is_object()) {
            ModDependencies deps;
            for (auto& [key, val] : j["optionalDependencies"].items())
                deps.insert(QString::fromStdString(key), QString::fromStdString(val.get<std::string>()));
            meta->optionalDependencies = deps;
        }

        // 贡献者（组装成 HTML）
        if (j.contains("contributors") && j["contributors"].is_array()) {
            QString html;
            for (const auto& item : j["contributors"]) {
                QString name = QString::fromStdString(item.value("name", "Unknown"));
                QString url = QString::fromStdString(item.value("url", ""));
                QString roles = QString::fromStdString(
                    item.contains("roles") ? item["roles"].get<std::string>() :
                    item.contains("role")  ? item["role"].get<std::string>()  : "");
                html += "<p><b>" + name + "</b>"
                    + (roles.isEmpty() ? "" : "<br/><font color='#AAAAAA'>" + roles + "</font>")
                    + (url.isEmpty()  ? "" : "<br/><a href='" + url + "'>" + url + "</a>")
                    + "</p>";
            }
            meta->contributors = html;
        }

        LOG_INFO("成功解析 VS 模组元数据: " + meta->id.value_or("?"));
    } catch (const std::exception& e) {
        LOG_ERROR(true, QString("解析VS元数据异常: ") + e.what());
    }

    return meta;
}

// ==================== 角色扫描 ====================

std::vector<VSModScanner::CharacterPair>
VSModScanner::scanCharacters(const QString& modPath)
{
    std::vector<CharacterPair> results;

    QString charDir = getDataPath(modPath, "characters");
    if (!QDir(charDir).exists()) return results;

    auto files = FileUtil::getFileAbsolutePaths(charDir, ".json");
    if (!files) return results;

    for (const auto& file : files.value()) {
        try {
            QString raw = FileUtil::ReadFileToString(file);
            auto j = json::parse(raw.toStdString());

            auto cd = std::make_unique<CharacterData>();
            cd->from_json(j);

            if (cd->assetPath.isEmpty() || cd->animations.isEmpty()) {
                LOG_WARNING(false, file + " 缺少必填字段");
                continue;
            }

            QString id = FileUtil::fetchIdFromFileName(file);
            LOG_INFO("VS 角色: " + id);
            results.push_back({id, std::move(cd)});
        } catch (const std::exception& e) {
            LOG_JSON_PARSE_ERROR(true, e.what(), file);
        }
    }

    return results;
}

// ==================== 歌曲扫描 ====================

std::vector<VSModScanner::SongPair>
VSModScanner::scanSongs(const QString& modPath)
{
    std::vector<SongPair> results;

    QString songsRoot = getDataPath(modPath, "songs");
    QDir dir(songsRoot);
    if (!dir.exists()) return results;

    QStringList songFolders = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const auto& folder : songFolders) {
        auto song = parseOneSong(modPath, folder);
        if (song)
            results.push_back({folder, std::move(song)});
    }

    return results;
}

std::unique_ptr<Song> VSModScanner::parseOneSong(const QString& modPath, const QString& songId)
{
    QString songPath = QDir::cleanPath(getDataPath(modPath, "songs") + "/" + songId);

    try {
        // 元数据
        QString metaFile = FileUtil::getDefaultSongMetaFilePath(songPath);
        if (!QFile::exists(metaFile)) {
            LOG_WARNING(false, songId + " 没有 metadata.json");
            return nullptr;
        }

        json jMeta = json::parse(FileUtil::ReadFileToString(metaFile).toStdString());
        SongMetaData defaultMeta;
        defaultMeta.from_json(jMeta);

        // 谱面
        QString chartFile = FileUtil::getDefaultSongChartFilePath(songPath);
        if (!QFile::exists(chartFile)) {
            LOG_WARNING(false, songId + " 没有 chart.json");
            return nullptr;
        }

        json jChart = json::parse(FileUtil::ReadFileToString(chartFile).toStdString());
        SongChartData defaultChart;
        defaultChart.from_json(jChart);

        // 构造 Song 对象
        auto song = std::make_unique<Song>(songId);
        song->addVariationSong("default", defaultMeta, defaultChart);

        // 解析变体
        for (const auto& varId : defaultMeta.playData.songVariations) {
            QString varMetaPath = FileUtil::getVariationSongMetaFilePath(songPath, varId);
            QString varChartPath = FileUtil::getVariationSongChartFilePath(songPath, varId);

            if (QFile::exists(varMetaPath) && QFile::exists(varChartPath)) {
                json jvMeta = json::parse(FileUtil::ReadFileToString(varMetaPath).toStdString());
                json jvChart = json::parse(FileUtil::ReadFileToString(varChartPath).toStdString());

                SongMetaData varMeta;
                SongChartData varChart;
                varMeta.from_json(jvMeta);
                varChart.from_json(jvChart);

                song->addVariationSong(varId, varMeta, varChart);
                LOG_INFO("  变体: " + varId);
            }
        }

        LOG_INFO("VS 歌曲: " + songId
                 + " (变体: " + QString::number(defaultMeta.playData.songVariations.size()) + "个)");
        return song;

    } catch (const std::exception& e) {
        LOG_JSON_PARSE_ERROR(true, e.what(), songPath);
        return nullptr;
    }
}

// ==================== 关卡扫描 ====================

std::vector<VSModScanner::LevelPair>
VSModScanner::scanLevels(const QString& modPath)
{
    std::vector<LevelPair> results;

    QString levelDir = getDataPath(modPath, "levels");
    if (!QDir(levelDir).exists()) return results;

    auto files = FileUtil::getFileAbsolutePaths(levelDir, ".json");
    if (!files) return results;

    for (const auto& file : files.value()) {
        try {
            QString raw = FileUtil::ReadFileToString(file);
            json j = json::parse(raw.toStdString());

            auto ld = std::make_unique<LevelData>();
            if (!ld->from_json(j)) continue;

            QString id = FileUtil::fetchIdFromFileName(file);
            LOG_INFO("VS 关卡: " + id);
            results.push_back({id, std::move(ld)});
        } catch (const std::exception& e) {
            LOG_JSON_PARSE_ERROR(true, e.what(), file);
        }
    }

    return results;
}

// ==================== 舞台扫描 ====================

std::vector<VSModScanner::StagePair>
VSModScanner::scanStages(const QString& modPath)
{
    std::vector<StagePair> results;

    QString stageDir = getDataPath(modPath, "stages");
    if (!QDir(stageDir).exists()) return results;

    auto files = FileUtil::getFileAbsolutePaths(stageDir, ".json");
    if (!files) return results;

    for (const auto& file : files.value()) {
        try {
            QString raw = FileUtil::ReadFileToString(file);
            json j = json::parse(raw.toStdString());

            auto sd = std::make_unique<StageData>();
            sd->from_json(j);

            QString id = FileUtil::fetchIdFromFileName(file);
            LOG_INFO("VS 舞台: " + id);
            results.push_back({id, std::move(sd)});
        } catch (const std::exception& e) {
            LOG_JSON_PARSE_ERROR(true, e.what(), file);
        }
    }

    return results;
}

// ==================== 箭头皮肤扫描 ====================

std::vector<VSModScanner::NoteStylePair>
VSModScanner::scanNoteStyles(const QString& modPath)
{
    std::vector<NoteStylePair> results;

    QString nsDir = getDataPath(modPath, "notestyles");
    if (!QDir(nsDir).exists()) return results;

    auto files = FileUtil::getFileAbsolutePaths(nsDir, ".json");
    if (!files) return results;

    for (const auto& file : files.value()) {
        try {
            QString raw = FileUtil::ReadFileToString(file);
            json j = json::parse(raw.toStdString());

            auto ns = std::make_unique<NoteStyleData>();
            ns->from_json(j);

            QString id = FileUtil::fetchIdFromFileName(file);
            LOG_INFO("VS 箭头皮肤: " + id);
            results.push_back({id, std::move(ns)});
        } catch (const std::exception& e) {
            LOG_JSON_PARSE_ERROR(true, e.what(), file);
        }
    }

    return results;
}

// ==================== 辅助 ====================

QString VSModScanner::getDataPath(const QString& modPath, const QString& subDir)
{
    return QDir::cleanPath(modPath + "/data/" + subDir);
}
