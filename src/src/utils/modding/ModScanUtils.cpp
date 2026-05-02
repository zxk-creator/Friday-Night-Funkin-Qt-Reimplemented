//
// Created by kkplay on 4/16/26.
//
#include "utils/modding/ModScanUtils.h"
#include "../thirdParty/nlohmann/json.hpp"
#include "data/character/CharacterData.h"
#include "data/character/CharacterRegistry.h"
#include "utils/exception/CustomException.h"
#include "data/level/LevelData.h"
#include "utils/VersionUtil.h"
#include "data/level/LevelRegistry.h"
#include "data/song/SongData.h"
#include "data/song/SongRegistry.h"
#include "play/song/Song.h"
#include "utils/file/FileUtil.h"
#include "utils/Path.h"
#include "utils/lang/LangStringPool.h"
#include "utils/message/MessageHandler.h"

using std::unordered_map;
using std::unique_ptr;
using std::make_unique;

ModEngineType ModScanUtils::judgeModEngine(const QString& modAbsolutePath)
{
    QDir modDir(modAbsolutePath);
    QFileInfo modInfo(modDir.absolutePath());
    MessageHandler::logInfo("传入的路径是: " + modDir.absolutePath());
    QString PECoreCfg = "pack.json";
    QString VSCoreCfg = "_polymod_meta.json";
    if (QFile::exists(modDir.filePath(PECoreCfg)))
    {
        MessageHandler::logInfo(modInfo.fileName() + "是PE模组！");
        return ModEngineType::PE;
    }
    if (QFile::exists(modDir.filePath(VSCoreCfg)))
    {
        MessageHandler::logInfo(modInfo.fileName() + "是V-Slice模组！");
        return ModEngineType::VS;
    }

    MessageHandler::logWarning(modInfo.fileName() + "没找到合适的配置文件！因此我默认他为PE。");
    return ModEngineType::PE;
}

// 真正意义上解析模组！
// TODO: Linux上大小写敏感，icon.png和icon.PNG完全是两个东西。等出问题了再修吧:)
optional<ModMetadata> ModScanUtils::scanPEModMetadata(QString& modAbsolutePath)
{
    ModMetadata modMetadata{};
    modMetadata.modPath = modAbsolutePath;
    modMetadata.engineType = ModEngineType::PE;

    QDir modDir(modAbsolutePath);
    QString cfgPath = modDir.filePath("pack.json");
    if (!QFile::exists(cfgPath))
    {
        MessageHandler::logWarning(modAbsolutePath + "没有pack.json!不过我仍旧尝试解析他...");
        return modMetadata;
    }

    QFile cfgFile(cfgPath);
    QFileInfo dirName(modDir.absolutePath());
    if (!cfgFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        MessageHandler::logWarning(dirName.fileName() + "无法解析pack.json文件，不过我仍旧尝试解析他");
        return modMetadata;
    }

    try {
        // 真正解析模组
        string rawJSON = cfgFile.readAll().toStdString();
        // 忽略注释和多余逗号
        auto j = nlohmann::json::parse(rawJSON, nullptr, true, true);

        modMetadata.title = QString::fromStdString(j.value("name", LangStringPool::instance()->untitledMod().toStdString()));
        modMetadata.description = QString::fromStdString(j.value("description", LangStringPool::instance()->noDescription().toStdString()));
        modMetadata.restart = j.value("restart", false);
        modMetadata.runsGlobally = j.value("runsGlobally", false);

        // 修复 bgRGBColor 解析
        if (j.contains("color") && j["color"].is_array()) {
            modMetadata.bgRGBColor = j["color"].get<vector<int>>();
        }

        QString normalIconPath = modDir.filePath("pack.png");
        QString pixelIconPath = modDir.filePath("pack-pixel.png");
        QFileInfo normalIconInfo(normalIconPath);
        QFileInfo pixelIconInfo(pixelIconPath);
        if (normalIconInfo.exists())
            modMetadata.iconPath = normalIconPath;
        else if (pixelIconInfo.exists())
            modMetadata.iconPath = pixelIconPath;
        else
            modMetadata.iconPath = nullopt;

        qInfo() << modMetadata.toString();
        return modMetadata;
    }
    catch (const std::exception& e)
    {
        MessageHandler::logError(QString("解析" + modAbsolutePath + "时发生异常：") + e.what() + "已跳过。" + "\n");
        return nullopt;
    }
}

// TODO: Linux上大小写敏感，icon.png和icon.PNG完全是两个东西。等出问题了再修吧:)
optional<ModMetadata> ModScanUtils::scanVSModMetadata(QString& modAbsolutePath)
{
    ModMetadata modMetadata{};
    modMetadata.modPath = modAbsolutePath;
    modMetadata.engineType = ModEngineType::VS;

    QDir modDir(modAbsolutePath);
    QString cfgPath = modDir.filePath("_polymod_meta.json");
    QString iconPath = modDir.filePath("_polymod_icon.png");
    if (!QFile::exists(cfgPath))
    {
        MessageHandler::logWarning(modAbsolutePath + "没有_polymod_meta.json！此模组无效。");
        return nullopt;
    }

    QFile cfgFile(cfgPath);
    QFileInfo dirName(modDir.absolutePath());
    QFileInfo iconInfo(iconPath);
    if (!cfgFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        MessageHandler::logWarning(dirName.fileName() + "无法解析_polymod_meta.json文件！此模组无效。");
        return nullopt;
    }

    try {
        string rawJSON = cfgFile.readAll().toStdString();
        auto j = nlohmann::json::parse(rawJSON, nullptr, true, true);

        modMetadata.title = QString::fromStdString(j.value("title", LangStringPool::instance()->untitledMod().toStdString()));
        modMetadata.description = QString::fromStdString(j.value("description", LangStringPool::instance()->noDescription().toStdString()));

        // 修复 homepage - std::optional 的问题
        if (j.contains("homepage") && j["homepage"].is_string()) {
            modMetadata.homepage = QString::fromStdString(j["homepage"].get<string>());
        }

        // 修复 apiVersion
        if (j.contains("api_version") && j["api_version"].is_string()) {
            QString apiVer = QString::fromStdString(j["api_version"].get<string>());
            modMetadata.apiVersion = "V-Slice " + apiVer;
        }

        // 修复 modVersion
        if (j.contains("mod_version") && j["mod_version"].is_string()) {
            modMetadata.modVersion = QString::fromStdString(j["mod_version"].get<string>());
        }

        // 修复 license
        if (j.contains("license") && j["license"].is_string()) {
            modMetadata.license = QString::fromStdString(j["license"].get<string>());
        }

        // 处理模组图标
        if (iconInfo.exists())
        {
            MessageHandler::logInfo(dirName.fileName() + "有图标!");
            modMetadata.iconPath = iconPath;
        }
        else
        {
            MessageHandler::logInfo(dirName.fileName() + "没有图标！");
        }

        // 处理dependencies - 直接用 QString 键值对
        if (j.contains("dependencies") && j["dependencies"].is_object())
        {
            ModDependencies dependencyMap;
            for (auto& [key, val] : j["dependencies"].items()) {
                dependencyMap.insert(QString::fromStdString(key), QString::fromStdString(val.get<string>()));
            }
            modMetadata.dependencies = dependencyMap;
        }

        // 处理optionalDependencies
        if (j.contains("optionalDependencies") && j["optionalDependencies"].is_object())
        {
            ModDependencies dependencyMap;
            for (auto& [key, val] : j["optionalDependencies"].items()) {
                dependencyMap.insert(QString::fromStdString(key), QString::fromStdString(val.get<string>()));
            }
            modMetadata.optionalDependencies = dependencyMap;
        }

        // 处理嵌套的contributor数组，拼接成完整字符串！
        if (j.contains("contributors") && j["contributors"].is_array())
        {
            // 防止nullopt导致崩溃
            QString currentStr = modMetadata.contributors.value_or(QString(""));

            // 直接使用html进行类似网页设计！
            for (const auto& item : j["contributors"]) {
                QString name = QString::fromStdString(item.value("name", "Unknown"));
                QString url = QString::fromStdString(item.value("url", ""));
                QString entry;

                if (item.contains("roles"))
                {
                    QString roles = QString::fromStdString(item.value("roles", ""));
                    entry =
                    "<p style='margin-bottom: 10px;'>"
                    "<b><font size='1' color='#FFFFFF'>" + name + "</font></b><br/>" +
                    "<font color='#AAAAAA'>" + roles + "</font><br/>";
                }
                else if (item.contains("role"))
                {
                    QString role = QString::fromStdString(item.value("role", ""));
                    entry =
                    "<p style='margin-bottom: 10px;'>"
                    "<b><font size='1' color='#FFFFFF'>" + name + "</font></b><br/>" +
                    "<font color='#AAAAAA'>" + role + "</font><br/>";
                }
                else
                {
                    entry =
                    "<p style='margin-bottom: 10px;'>"
                    "<b><font size='1' color='#FFFFFF'>" + name + "</font></b><br/>";
                }

                if (!url.isEmpty()) {
                    entry += "<a href='" + url + "' style='color: #2196F3;'>" + url + "</a>";
                }

                entry += "</p>";
                currentStr += entry;
            }
            modMetadata.contributors = currentStr;
        }

        qInfo() << modMetadata.toString();

        return modMetadata;
    }
    catch (const std::exception& e)
    {
        MessageHandler::logError(QString("解析" + modAbsolutePath + "时发生异常：") + e.what() + "已跳过。" + "\n");
        return nullopt;
    }
}

void ModScanUtils::scanAllMods(const QVector<ModMetadata>& modMetadatas)
{
    for (auto& entry : modMetadatas)
    {
        string levelName;
        // 按顺序来，不能错！
        if (parseWeeks(entry))
            MessageHandler::logWarning("解析" + entry.modPath + "时出现问题，已跳过。");

        parseSongs(entry);
        parseStages(entry);
    }
}

void ModScanUtils::scanOneMod(const ModMetadata& modMetadata)
{
    parseWeeks(modMetadata);
    parseSongs(modMetadata);
}

bool ModScanUtils::parseWeeks(const ModMetadata& modMetadata)
{
    QString modAbsolutePath = modMetadata.modPath;
    QString finalLevelDirPath = Path::getVSDataPath(DataResourceType::levels,modAbsolutePath);

    optional<QStringList> jsonFiles = FileUtil::getFileNames(finalLevelDirPath,"*.json");
    if (!jsonFiles.has_value()) return false;

    foreach(const QString& fileName, jsonFiles.value())
    {
        QString oneJSONFile = finalLevelDirPath + QDir::separator() + fileName;
        // 直接开始解析json
        QString content = FileUtil::ReadFileToString(oneJSONFile);

        // 由于我们已经写好了from_json函数，因此可以直接获取！
        json j = nlohmann::json::parse(content.toStdString());
        optional<LevelData> levelData = LevelDataParser::parseLevelData_VS(j,fileName);
        if (!levelData.has_value()) continue;

        auto l_ptr = make_unique<LevelData>();
        LevelRegistry::instance()->addEntry(levelData.value().name,std::move(l_ptr));
    }

    return true;
}

bool ModScanUtils::parseSongs(const ModMetadata& modMetadata)
{
    QString modAbsolutePath = modMetadata.modPath;
    // 接下来我们就得去songs目录下去找json了
    QString songsRootPath = Path::getVSDataPath(DataResourceType::songs, modAbsolutePath);
    QDir dir(songsRootPath);
    QStringList songsSubFolders = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString& songIdAndFolderName : songsSubFolders) {
        QString thisSongDataFolderPath = songsRootPath + QDir::separator() + songIdAndFolderName;
        QString defaultSongMetaFilePath = FileUtil::getDefaultSongMetaFilePath(thisSongDataFolderPath);

        QString thisMetaFileName = FileUtil::getPathLeaf(defaultSongMetaFilePath);
        QString thisChartFilePath = FileUtil::getDefaultSongChartFilePath(thisSongDataFolderPath);
        try {
            // 先解析普通元数据文件
            json jMeta = json::parse(FileUtil::ReadFileToString(defaultSongMetaFilePath.toStdString()));
            optional<SongMetaData> defaultMetadata = SongDataParser::parseSongMetaData(jMeta,thisMetaFileName);
            // 先解析普通铺面文件
            json jChart = json::parse(FileUtil::ReadFileToString(thisChartFilePath).toStdString());
            SongChartData defaultChartData = SongDataParser::parseSongChartData(jChart,thisMetaFileName);

            if (!defaultMetadata.has_value()) continue;
            // 获得歌曲的变体名，并对其进行遍历
            QVector<QString> songVariations = defaultMetadata.value().playData.songVariations;

            // 根据SongCharacterData, 找到对应的ogg文件
            QString playerId = defaultMetadata.value().playData.characters.player;
            QString opponentId = defaultMetadata.value().playData.characters.opponent;

            // 构建路径（尚未使用，原先想让metadata存储歌曲路径的，想想还是存id，动态拼接吧。）
            QString defaultPlayerVocalPath = FileUtil::getSongFullPath(modAbsolutePath,false,playerId,"",songIdAndFolderName);
            QString defaultOpponentVocalPath = FileUtil::getSongFullPath(modAbsolutePath,false,opponentId,"",songIdAndFolderName);
            QString defaultInstPath = FileUtil::getSongFullPath(modAbsolutePath,true,"","","");

            auto song = make_unique<Song>(songIdAndFolderName);
            song->addVariationSong("default",defaultMetadata.value(),defaultChartData);

            // 遍历变体
            for (auto& songVariationId : songVariations)
            {
                try {
                    // 变体不能嵌套变体！这是代码里面硬性规定的！
                    QString thisVariationMetadataPath = FileUtil::getVariationSongMetaFilePath(thisSongDataFolderPath,songVariationId);
                    QString thisVariationChartPath = FileUtil::getVariationSongChartFilePath(thisSongDataFolderPath,songVariationId);

                    // erect变体只有erect或nightmare难度（目前）
                    json jvmetadata = json::parse(FileUtil::ReadFileToString(thisVariationMetadataPath.toStdString()));
                    optional<SongMetaData> variationMetadata = SongDataParser::parseSongMetaData(jvmetadata,thisVariationMetadataPath);

                    if (!variationMetadata.has_value()) continue;
                    // 音频文件路径（未使用）
                    QString variationPlayerVoicePath = FileUtil::getSongFullPath(modAbsolutePath,false,playerId,songVariationId,songIdAndFolderName);
                    QString variationOpponentPath = FileUtil::getSongFullPath(modAbsolutePath,false,opponentId,songVariationId,songIdAndFolderName);
                    QString variationInstPath = FileUtil::getSongFullPath(modAbsolutePath,true,"",songVariationId,songIdAndFolderName);

                    json jvChartData = json::parse(FileUtil::ReadFileToString(thisVariationChartPath.toStdString()));
                    SongChartData variationChartData = SongDataParser::parseSongChartData(jvChartData,thisVariationChartPath);

                    // 存储变体与铺面
                    song->addVariationSong(songVariationId,variationMetadata.value(),variationChartData);
                }
                catch (json::parse_error& e)
                {
                    LOG_JSON_PARSE_ERROR(e.what(),thisMetaFileName);
                    continue;
                }
            }

            SongRegistry::instance()->addEntry(songIdAndFolderName,std::move(song));
            // 至此，我们终于完成了一首歌的构造！🎉🎉🎉🎉
        }
        catch (const json::parse_error& e)
        {
            QString why = e.what();
            QString res = "解析JSON时发生异常" + why;
            MessageHandler::logError(why + res + "文件: " + songIdAndFolderName);
            continue;
        }
    }

    return true;
}

bool ModScanUtils::parseCharacters(const ModMetadata& modMetadata)
{
    QString modAbsolutePath = modMetadata.modPath;
    // 思路：找到data/character目录下的所有json文件并递归解析
    QString characterDataPath = Path::getVSDataPath(DataResourceType::character,modAbsolutePath);
    optional<QStringList> allJsonFiles = FileUtil::getFileAbsolutePaths(characterDataPath,".json");
    if (!allJsonFiles.has_value()) return false;

    for (auto& file : allJsonFiles.value())
    {
        try
        {
            QString rawJSON = FileUtil::ReadFileToString(file);
            json j = json::parse(rawJSON.toStdString());
            auto cd = parseCharacterData(j,file);
            // 万一里面解析出问题，返回空指针，那完蛋了
            // 突然想用rust了
            if (cd == nullptr) continue;

            QString id = FileUtil::fetchIdFromFileName(file);
            CharacterRegistry::instance()->addEntry(id,std::move(cd));
        }
        catch (const json::parse_error& e)
        {
            QString why = e.what();
            QString res = "解析JSON时发生异常" + why;
            MessageHandler::logError(why + res + "文件: " + file);
            continue;
        }
    }
}

bool ModScanUtils::parseStages(const ModMetadata& modMetadata)
{

}

bool ModScanUtils::parseNotestyles(const ModMetadata& modMetadata)
{

}
