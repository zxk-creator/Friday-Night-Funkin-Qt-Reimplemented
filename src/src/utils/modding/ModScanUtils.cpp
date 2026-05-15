//
// Created by kkplay on 4/16/26.
//
#include "utils/modding/ModScanUtils.h"
#include "../thirdParty/nlohmann/json.hpp"
#include "data/Context.h"
#include "data/character/CharacterData.h"
#include "data/character/CharacterRegistry.h"
#include "data/level/LevelData.h"
#include "data/mod/ModRegistry.h"
#include "data/notestyle/NoteStyleData.h"
#include "data/song/SongData.h"
#include "data/song/SongRegistry.h"
#include "data/stage/StageData.h"
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
    MessageHandler::logInfo("传入的路径是: " + modDir.absolutePath(), "ModScanUtils");
    QString PECoreCfg = "pack.json";
    QString VSCoreCfg = "_polymod_meta.json";
    if (QFile::exists(modDir.filePath(PECoreCfg)))
    {
        MessageHandler::logInfo(modInfo.fileName() + "是PE模组！", "ModScanUtils");
        return ModEngineType::PE;
    }
    if (QFile::exists(modDir.filePath(VSCoreCfg)))
    {
        MessageHandler::logInfo(modInfo.fileName() + "是V-Slice模组！", "ModScanUtils");
        return ModEngineType::VS;
    }

    MessageHandler::logWarning(false, modInfo.fileName() + "没找到合适的配置文件！因此我默认他为PE。", "ModScanUtils");
    return ModEngineType::PE;
}

// 真正意义上解析模组！
// TODO: Linux上大小写敏感，icon.png和icon.PNG完全是两个东西。等出问题了再修吧:)
void ModScanUtils::scanPEModMetadata(const QString& modAbsolutePath)
{
    auto modMetadata = make_unique<ModMetadata>();
    modMetadata->modPath = modAbsolutePath;
    modMetadata->engineType = ModEngineType::PE;

    QDir modDir(modAbsolutePath);
    QString cfgPath = modDir.filePath("pack.json");
    if (!QFile::exists(cfgPath))
        MessageHandler::logWarning(false, modAbsolutePath + "没有pack.json!不过我仍旧尝试解析他...", "ModScanUtils");


    QFile cfgFile(cfgPath);
    QFileInfo dirName(modDir.absolutePath());
    if (!cfgFile.open(QIODevice::ReadOnly | QIODevice::Text))
        MessageHandler::logWarning(false, dirName.fileName() + "无法解析pack.json文件，不过我仍旧尝试解析他", "ModScanUtils");

    try
    {
        // 真正解析模组
        string rawJSON = cfgFile.readAll().toStdString();
        // 忽略注释和多余逗号
        auto j = nlohmann::json::parse(rawJSON, nullptr, true, true);

        modMetadata->title = QString::fromStdString(
            j.value("name", LangStringPool::instance()->untitledMod().toStdString()));
        modMetadata->description = QString::fromStdString(
            j.value("description", LangStringPool::instance()->noDescription().toStdString()));
        modMetadata->restart = j.value("restart", false);
        modMetadata->runsGlobally = j.value("runsGlobally", false);

        optional<QString> id = FileUtil::getPathLeaf(modAbsolutePath);
        if (!id.has_value()) id = "Unnamed";
        modMetadata->id = id;

        if (j.contains("color") && j["color"].is_array())
        {
            modMetadata->bgRGBColor = j["color"].get<vector<int>>();
        }

        QString normalIconPath = modDir.filePath("pack.png");
        QString pixelIconPath = modDir.filePath("pack-pixel.png");
        QFileInfo normalIconInfo(normalIconPath);
        QFileInfo pixelIconInfo(pixelIconPath);
        if (normalIconInfo.exists())
            modMetadata->iconPath = normalIconPath;
        else if (pixelIconInfo.exists())
            modMetadata->iconPath = pixelIconPath;
        else
            modMetadata->iconPath = nullopt;

        MessageHandler::logInfo(modMetadata->toString(), "ModScanUtils");

        // 最终注册
        Context::modRegistry->addEntry(id.value(), std::move(modMetadata));
    }
    catch (const std::exception& e)
    {
        MessageHandler::logError(false, QString("解析" + modAbsolutePath + "时发生异常：") + e.what() + "已跳过。" + "\n",
                                 "ModScanUtils");
    }
}

// TODO: Linux上大小写敏感，icon.png和icon.PNG完全是两个东西。等出问题了再修吧:)
void ModScanUtils::scanVSModMetadata(const QString& modAbsolutePath)
{
    auto modMetadata = make_unique<ModMetadata>();
    modMetadata->modPath = modAbsolutePath;
    modMetadata->engineType = ModEngineType::VS;

    // 我们把模组的文件夹作为他的id！PE和VS都是这么做的。
    optional<QString> id = FileUtil::getPathLeaf(modAbsolutePath);
    if (!id.has_value()) id = "Unnamed";
    modMetadata->id = id;

    QDir modDir(modAbsolutePath);
    QString cfgPath = modDir.filePath("_polymod_meta.json");
    QString iconPath = modDir.filePath("_polymod_icon.png");
    if (!QFile::exists(cfgPath))
    {
        MessageHandler::logWarning(false, modAbsolutePath + "没有_polymod_meta.json！此模组无效。", "ModScanUtils");
    }

    QFile cfgFile(cfgPath);
    QFileInfo dirName(modDir.absolutePath());
    QFileInfo iconInfo(iconPath);
    if (!cfgFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        MessageHandler::logWarning(false, dirName.fileName() + "无法解析_polymod_meta.json文件！此模组无效。", "ModScanUtils");
    }

    try
    {
        string rawJSON = cfgFile.readAll().toStdString();
        auto j = nlohmann::json::parse(rawJSON, nullptr, true, true);

        modMetadata->title = QString::fromStdString(
            j.value("title", LangStringPool::instance()->untitledMod().toStdString()));
        modMetadata->description = QString::fromStdString(
            j.value("description", LangStringPool::instance()->noDescription().toStdString()));

        if (j.contains("homepage") && j["homepage"].is_string())
        {
            modMetadata->homepage = QString::fromStdString(j["homepage"].get<string>());
        }

        if (j.contains("api_version") && j["api_version"].is_string())
        {
            QString apiVer = QString::fromStdString(j["api_version"].get<string>());
            modMetadata->apiVersion = "V-Slice " + apiVer;
        }

        if (j.contains("mod_version") && j["mod_version"].is_string())
        {
            modMetadata->modVersion = QString::fromStdString(j["mod_version"].get<string>());
        }

        if (j.contains("license") && j["license"].is_string())
        {
            modMetadata->license = QString::fromStdString(j["license"].get<string>());
        }

        if (iconInfo.exists())
        {
            MessageHandler::logInfo(dirName.fileName() + "有图标!", "ModScanUtils");
            modMetadata->iconPath = iconPath;
        }
        else
        {
            MessageHandler::logInfo(dirName.fileName() + "没有图标！", "ModScanUtils");
        }

        if (j.contains("dependencies") && j["dependencies"].is_object())
        {
            ModDependencies dependencyMap;
            for (auto& [key, val] : j["dependencies"].items())
            {
                dependencyMap.insert(QString::fromStdString(key), QString::fromStdString(val.get<string>()));
            }
            modMetadata->dependencies = dependencyMap;
        }

        if (j.contains("optionalDependencies") && j["optionalDependencies"].is_object())
        {
            ModDependencies dependencyMap;
            for (auto& [key, val] : j["optionalDependencies"].items())
            {
                dependencyMap.insert(QString::fromStdString(key), QString::fromStdString(val.get<string>()));
            }
            modMetadata->optionalDependencies = dependencyMap;
        }

        // 处理嵌套的contributor数组，拼接成完整字符串！
        if (j.contains("contributors") && j["contributors"].is_array())
        {
            // 防止nullopt导致崩溃
            QString currentStr = modMetadata->contributors.value_or(QString(""));

            // 直接使用html进行类似网页设计！
            for (const auto& item : j["contributors"])
            {
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

                if (!url.isEmpty())
                {
                    entry += "<a href='" + url + "' style='color: #2196F3;'>" + url + "</a>";
                }

                entry += "</p>";
                currentStr += entry;
            }
            modMetadata->contributors = currentStr;
        }

        // qInfo() << modMetadata->toString();
        // 注册！
        Context::modRegistry->addEntry(id.value(), std::move(modMetadata));
        LOG_INFO("成功解析了模组元数据: " + id.value());
    }
    catch (const std::exception& e)
    {
        MessageHandler::logError(false, QString("解析" + modAbsolutePath + "时发生异常：") + e.what() + "已跳过。" + "\n",
                                 "ModScanUtils");
    }
}

void ModScanUtils::scanAllMods(const QVector<ModMetadata>& modMetadatas)
{
    for (auto& entry : modMetadatas)
    {
        string levelName;
        // 按顺序来，不能错！
        if (parseWeeks(entry))
            MessageHandler::logWarning(false, "解析" + entry.modPath + "时出现问题，已跳过。", "ModScanUtils");

        parseSongs(entry);
        parseStages(entry);
    }
}

void ModScanUtils::scanOneMod(const ModMetadata& modMetadata)
{
    parseWeeks(modMetadata);
    parseSongs(modMetadata);
    parseCharacters(modMetadata);
    parseStages(modMetadata);
    parseNotestyles(modMetadata);
}

bool ModScanUtils::parseWeeks(const ModMetadata& modMetadata)
{
    QString modAbsolutePath = modMetadata.modPath;
    switch (modMetadata.engineType)
    {
    case ModEngineType::VS:
        {
            QString finalLevelDirPath = PathVS::getDataPath(EDataResourceType::levels, modAbsolutePath);

            optional<QStringList> jsonFiles = FileUtil::getFileAbsolutePaths(finalLevelDirPath, "*.json");
            if (!jsonFiles.has_value()) return false;

            for (auto& filePath : jsonFiles.value())
            {
                // 直接开始解析json
                QString content = FileUtil::ReadFileToString(filePath);
                QString id = FileUtil::fetchIdFromFileName(filePath);

                // 由于我们已经写好了from_json函数，因此可以直接获取！
                json j = nlohmann::json::parse(content.toStdString());
                auto levelData = LevelDataParser::parseLevelData_VS(j, filePath);
                if (!levelData) continue;

                // 写入缓冲区
                Context::modRegistry->getCache()->levelRelative = levelData->toString();

                Context::levelRegistry->addEntry(id, std::move(levelData));
                LOG_INFO("成功解析了关卡: " + filePath);
            }
            break;
        }
    case ModEngineType::PE:
        {
            QVector<QString> PELevelDirPath = PathPE::getDataPath(EDataResourceType::levels, modAbsolutePath);
            for (auto& entry : PELevelDirPath)
            {
                optional<QStringList> jsonFiles = FileUtil::getFileAbsolutePaths(entry, "*.json");
                if (!jsonFiles.has_value()) continue;

                // 然后获取里面所有的json文件
                for (auto& jsonFile : jsonFiles.value())
                {
                    QString rawContent = FileUtil::ReadFileToString(jsonFile);
                    try
                    {
                        json j = nlohmann::json::parse(rawContent.toStdString());
                        unique_ptr<LevelData> levelData = LevelDataParser::parseLevelData_PE(j, jsonFile);
                        if (!levelData) continue;

                        QString idnum = FileUtil::genUID();
                        QString id = FileUtil::fetchIdFromFileName(jsonFile) + idnum;

                        // 写入缓冲区
                        Context::modRegistry->getCache()->levelRelative = levelData->toString();
                        // PE要id其实没啥用
                        Context::levelRegistry->addEntry(id, std::move(levelData));
                        LOG_PARSE_JSON_SUCCESS("Week", jsonFile);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_JSON_PARSE_ERROR(false, e.what(), jsonFile);
                    }
                }
            }

            break;
        }
    default:
        {
            LOG_WARNING(false, "暂不支持的模组类型。");
            return false;
        }
    }

    return true;
}

bool ModScanUtils::parseSongs(const ModMetadata& modMetadata)
{
    QString modAbsolutePath = modMetadata.modPath;

    switch (modMetadata.engineType)
    {
    case ModEngineType::VS:
        {
            // 接下来我们就得去songs目录下去找json了
            QString songsRootPath = PathVS::getDataPath(EDataResourceType::songs, modAbsolutePath);
            QDir dir(songsRootPath);
            QStringList songsSubFolders = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

            for (const QString& songIdAndFolderName : songsSubFolders)
            {
                QString thisSongDataFolderPath = songsRootPath + QDir::separator() + songIdAndFolderName;
                QString defaultSongMetaFilePath = FileUtil::getDefaultSongMetaFilePath(thisSongDataFolderPath);

                optional<QString> thisMetaFileName = FileUtil::getPathLeaf(defaultSongMetaFilePath);
                if (!thisMetaFileName.has_value()) continue;

                QString thisChartFilePath = FileUtil::getDefaultSongChartFilePath(thisSongDataFolderPath);
                try
                {
                    // 先解析普通元数据文件
                    json jMeta = json::parse(FileUtil::ReadFileToString(defaultSongMetaFilePath.toStdString()));
                    optional<SongMetaData> defaultMetadata = SongDataParser::parseSongMetaData_VS(
                        jMeta, thisMetaFileName.value());
                    // 先解析普通铺面文件
                    json jChart = json::parse(FileUtil::ReadFileToString(thisChartFilePath).toStdString());
                    SongChartData defaultChartData = SongDataParser::parseSongChartData_VS(
                        jChart, thisMetaFileName.value());

                    if (!defaultMetadata.has_value()) continue;
                    // 获得歌曲的变体名，并对其进行遍历
                    QVector<QString> songVariations = defaultMetadata.value().playData.songVariations;

                    // 根据SongCharacterData, 找到对应的ogg文件
                    QString playerId = defaultMetadata.value().playData.characters.player;
                    QString opponentId = defaultMetadata.value().playData.characters.opponent;

                    // 构建路径（尚未使用，原先想让metadata存储歌曲路径的，想想还是存id，动态拼接吧。）
                    QString defaultPlayerVocalPath = FileUtil::getSongFullPath(
                        modAbsolutePath, false, playerId, "", songIdAndFolderName);
                    QString defaultOpponentVocalPath = FileUtil::getSongFullPath(
                        modAbsolutePath, false, opponentId, "", songIdAndFolderName);
                    QString defaultInstPath = FileUtil::getSongFullPath(modAbsolutePath, true, "", "", "");

                    auto song = make_unique<Song>(songIdAndFolderName);
                    song->addVariationSong("default", defaultMetadata.value(), defaultChartData);

                    // 遍历变体
                    for (auto& songVariationId : songVariations)
                    {
                        try
                        {
                            // 变体不能嵌套变体！这是代码里面硬性规定的！
                            QString thisVariationMetadataPath = FileUtil::getVariationSongMetaFilePath(
                                thisSongDataFolderPath, songVariationId);
                            QString thisVariationChartPath = FileUtil::getVariationSongChartFilePath(
                                thisSongDataFolderPath, songVariationId);

                            // erect变体只有erect或nightmare难度（目前）
                            json jvmetadata = json::parse(
                                FileUtil::ReadFileToString(thisVariationMetadataPath.toStdString()));
                            optional<SongMetaData> variationMetadata = SongDataParser::parseSongMetaData_VS(
                                jvmetadata, thisVariationMetadataPath);

                            if (!variationMetadata.has_value()) continue;
                            // 音频文件路径（未使用）
                            QString variationPlayerVoicePath = FileUtil::getSongFullPath(
                                modAbsolutePath, false, playerId, songVariationId, songIdAndFolderName);
                            QString variationOpponentPath = FileUtil::getSongFullPath(
                                modAbsolutePath, false, opponentId, songVariationId, songIdAndFolderName);
                            QString variationInstPath = FileUtil::getSongFullPath(
                                modAbsolutePath, true, "", songVariationId, songIdAndFolderName);

                            json jvChartData = json::parse(
                                FileUtil::ReadFileToString(thisVariationChartPath.toStdString()));
                            SongChartData variationChartData = SongDataParser::parseSongChartData_VS(
                                jvChartData, thisVariationChartPath);

                            // 存储变体与铺面
                            song->addVariationSong(songVariationId, variationMetadata.value(), variationChartData);
                        }
                        catch (json::parse_error& e)
                        {
                            MessageHandler::logError(
                                true, "解析JSON时发生异常: " + QString(e.what()) + "在" + songIdAndFolderName,
                                "ModScanUtils::parseSongs");
                            continue;
                        }
                    }

                    // 写入缓冲区
                    Context::modRegistry->getCache()->songRelative = song->toString();
                    Context::levelRegistry->addEntry(songIdAndFolderName, std::move(song));
                    // 至此，我们终于完成了一首歌的构造！🎉🎉🎉🎉
                    LOG_INFO("成功解析了歌曲铺面和元数据: " + songIdAndFolderName);
                }
                catch (const json::parse_error& e)
                {
                    LOG_JSON_PARSE_ERROR(true, e.what(), songIdAndFolderName);
                    continue;
                }
            }
            break;
        }
    case ModEngineType::PE:
        {
            QVector<QString> songDataPaths = PathPE::getDataPath(EDataResourceType::songs, modAbsolutePath);

            for (auto& songDataPath : songDataPaths)
            {
                QStringList oneSongDataPaths = Path::getSubDirs(songDataPath);
                LOG_INFO(QString("发现") + QString::number(oneSongDataPaths.size()) + "首歌曲(目录)!");

                QString songId;
                // 我们直接根据json文件来判断难度，不根据week里面声明的进行判断了。
                // 这里的路径就是json文件所存放的最终位置。
                for (auto& oneSongDataPath : oneSongDataPaths)
                {
                    songId = FileUtil::fetchIdFromFileName(oneSongDataPath);
                    LOG_INFO(QString("发现配置") + oneSongDataPath + "，id为" + songId);

                    QString upper = songId.toUpper();
                    QString lower = songId.toLower();
                    optional<QStringList> songChartDataPaths = FileUtil::getFileNames(oneSongDataPath, ".json");
                    if (!songChartDataPaths.has_value()) continue;
                    LOG_INFO(QString("一共找到了" + QString::number(songChartDataPaths->size()) + "个配置文件！"));

                    QVector<SongChartData> PEChartDatas;
                    QVector<SongEventData> PEEventDatas;
                    QVector<QString> difficultyArray;
                    // 随便一首歌的铺面
                    json oneChartData;
                    for (auto& songChartDataPath : songChartDataPaths.value())
                    {
                        QString fileName = FileUtil::getPathLeaf(songChartDataPath)->toLower();
                        // 有的还有单独的events.json，我们单独伶出来解析
                        if (fileName == "events.json")
                        {
                            try
                            {
                                json eventData = json::parse(
                                    FileUtil::ReadFileToString(songChartDataPath).toStdString());
                                PEEventDatas = SongDataParser::parseEvents_PE(eventData, songChartDataPath);
                                LOG_INFO("已找到配置文件events.json");
                                continue;
                            }
                            catch (const json::parse_error& e)
                            {
                                LOG_JSON_PARSE_ERROR(true, e.what(), songChartDataPath);
                                continue;
                            }
                        }

                        QString diffStr = FileUtil::getDifficultyStrFromFileName(songChartDataPath);
                        difficultyArray.push_back(diffStr);
                        try
                        {
                            json chartData = json::parse(FileUtil::ReadFileToString(songChartDataPath).toStdString());
                            oneChartData = chartData;
                            // 还需要获取同目录下的events.json数组进行解析，我放到外面了
                            PEChartDatas.push_back(SongDataParser::parseSongChartData_PE(diffStr, chartData));
                        }
                        catch (json::parse_error& e)
                        {
                            LOG_JSON_PARSE_ERROR(true, e.what(), songChartDataPath);
                        }
                    }
                    // 循环完成，这个目录下的所有Chart信息都已被解析完成，开始合并
                    // TODO: 需要注意的是，PE不同难度的歌能单独设置他的舞台背景，而官方引擎不支持
                    // 我们目前只是简单合并，所有歌曲共享同一个舞台，此问题等回头再修吧
                    SongChartData mergedSongChartData = SongDataParser::mergeChartData_PE(PEChartDatas, PEEventDatas);
                    auto song = make_unique<Song>(FileUtil::fetchIdFromFileName(songId));
                    SongMetaData PESongMetaData = SongDataParser::parseSongMetaData_PE(
                        difficultyArray, oneChartData, songDataPath);
                    song->addVariationSong("default", PESongMetaData, mergedSongChartData);

                    // 写入缓冲区
                    Context::modRegistry->getCache()->songRelative = song->toString();
                    Context::songRegistry->addEntry(songId, std::move(song));
                    LOG_INFO("成功解析了歌曲" + songId);
                }
            }
            break;
        }
    default:
        {
            LOG_WARNING(true, "暂不支持的模组类型。");
            return false;
        }
    }

    return true;
}

bool ModScanUtils::parseCharacters(const ModMetadata& modMetadata)
{
    QString modAbsolutePath = modMetadata.modPath;
    switch (modMetadata.engineType)
    {
    case ModEngineType::VS:
        {
            // 思路：找到data/character目录下的所有json文件并递归解析
            QString characterDataPath = PathVS::getDataPath(EDataResourceType::character, modAbsolutePath);
            optional<QStringList> allJsonFiles = FileUtil::getFileAbsolutePaths(characterDataPath, ".json");
            if (!allJsonFiles.has_value()) return false;

            for (auto& file : allJsonFiles.value())
            {
                try
                {
                    QString rawJSON = FileUtil::ReadFileToString(file);
                    json j = json::parse(rawJSON.toStdString());
                    auto cd = CharacterDataParser::parseCharacterData_VS(j, file);
                    // 万一里面解析出问题，返回空指针，那完蛋了
                    // 突然想用rust了
                    if (cd == nullptr) continue;

                    QString id = FileUtil::fetchIdFromFileName(file);

                    // 写入缓冲区
                    Context::modRegistry->getCache()->characterRelative = cd->toString();
                    Context::characterRegistry->addEntry(id, std::move(cd));
                    LOG_INFO("成功解析了V-Slice角色: " + id);
                }
                catch (const json::parse_error& e)
                {
                    LOG_JSON_PARSE_ERROR(true, e.what(), file);
                    continue;
                }
            }
            break;
        }
    case ModEngineType::PE:
        {
            QVector<QString> characterDataPaths = PathPE::getDataPath(EDataResourceType::character, modAbsolutePath);
            for (auto& characterDataPath : characterDataPaths)
            {
                optional<QStringList> allJsonFiles = FileUtil::getFileAbsolutePaths(characterDataPath, "*.json");
                if (!allJsonFiles.has_value()) continue;

                for (auto& oneJsonFile : allJsonFiles.value())
                {
                    QString rawJSON = FileUtil::ReadFileToString(oneJsonFile);
                    try
                    {
                        json j = json::parse(rawJSON.toStdString());
                        unique_ptr<CharacterData> characterData = CharacterDataParser::parseCharacterData_PE(
                            j, oneJsonFile, modAbsolutePath);
                        if (!characterData) continue;

                        QString id = FileUtil::fetchIdFromFileName(oneJsonFile);

                        Context::modRegistry->getCache()->characterRelative = characterData->toString();
                        Context::characterRegistry->addEntry(id, std::move(characterData));
                        LOG_INFO("成功解析了Psych Engine角色" + id);
                    }
                    catch (const json::parse_error& e)
                    {
                        LOG_JSON_PARSE_ERROR(true, e.what(), characterDataPath);
                    }
                }
            }
            break;
        }
    default:
        {
            LOG_WARNING(false, "赞不支持的模组类型");
        }
    }

    return true;
}

bool ModScanUtils::parseStages(const ModMetadata& modMetadata)
{
    QString modPath = modMetadata.modPath;
    switch (modMetadata.engineType)
    {
    case ModEngineType::VS:
        {
            QString modStageJsonPath = PathVS::getDataPath(EDataResourceType::stages, modPath);

            optional<QStringList> fileList = FileUtil::getFileAbsolutePaths(modStageJsonPath, ".json");
            if (!fileList.has_value()) return false;
            for (auto& filepath : fileList.value())
            {
                try
                {
                    QString rawJSON = FileUtil::ReadFileToString(filepath);
                    json j = json::parse(rawJSON.toStdString());
                    unique_ptr<StageData> stageData = StageDataParser::parseStageData_VS(j, filepath);
                    if (!stageData) continue;
                    QString id = FileUtil::fetchIdFromFileName(filepath);

                    Context::modRegistry->getCache()->stageRelative = stageData->toString();
                    Context::stageRegistry->addEntry(id, std::move(stageData));
                    LOG_INFO("成功解析了StageData: " + id);
                }
                catch (const json::parse_error& e)
                {
                    LOG_ERROR(true, QString("解析JSON" + filepath + "时发生异常: ") + e.what() + "已跳过。");
                }
            }
            break;
        }
    case ModEngineType::PE:
        {
            QStringList modStageJsonPaths = PathPE::getDataPath(EDataResourceType::stages, modPath);
            for (auto& modStageJsonPath : modStageJsonPaths)
            {
                optional<QStringList> fileList = FileUtil::getFileAbsolutePaths(modStageJsonPath, ".json");
                if (!fileList.has_value()) continue;
                for (auto& filepath : fileList.value())
                {
                    try
                    {
                        QString rawJSON = FileUtil::ReadFileToString(filepath);
                        json j = json::parse(rawJSON.toStdString());
                        unique_ptr<StageData> stageData = StageDataParser::parseStageData_PE(j, filepath);
                        if (!stageData) continue;
                        QString id = FileUtil::fetchIdFromFileName(filepath);

                        Context::modRegistry->getCache()->stageRelative = stageData->toString();
                        Context::stageRegistry->addEntry(id, std::move(stageData));
                    }
                    catch (const json::parse_error& e)
                    {
                        LOG_JSON_PARSE_ERROR(false, e.what(), filepath);
                    }
                }
            }
            break;
        }
    default:
        {
            LOG_WARNING(false, "不支持的模组版本");
        }
    }

    return true;
}

bool ModScanUtils::parseNotestyles(const ModMetadata& modMetadata)
{
    QString modPath = modMetadata.modPath;

    switch (modMetadata.engineType)
    {
    case ModEngineType::VS:
        {
            QString modStageJsonPath = PathVS::getDataPath(EDataResourceType::notestyles, modPath);

            optional<QStringList> fileList = FileUtil::getFileAbsolutePaths(modStageJsonPath, ".json");
            if (!fileList.has_value()) return false;
            for (auto& filepath : fileList.value())
            {
                try
                {
                    QString rawJSON = FileUtil::ReadFileToString(filepath);
                    json j = json::parse(rawJSON.toStdString());
                    unique_ptr<NoteStyleData> noteStyleData = NoteStyleParser::parseNoteStyleData_VS(j, filepath);
                    if (!noteStyleData) continue;
                    QString id = FileUtil::fetchIdFromFileName(filepath);

                    Context::modRegistry->getCache()->noteStyleRelative = noteStyleData->toString();
                    Context::noteStyleRegistry->addEntry(id, std::move(noteStyleData));

                    LOG_INFO("成功解析了V-Slice NoteStyle: " + id);
                }
                catch (const json::parse_error& e)
                {
                    LOG_ERROR(true, QString("解析JSON" + filepath + "时发生异常: ") + e.what() + "已跳过。");
                }
            }
            break;
        }
    case ModEngineType::PE:
        {
            QVector<QString> songRootPaths = PathPE::getDataPath(EDataResourceType::songs, modPath);
            QVector<QString> stageRootPaths = PathPE::getDataPath(EDataResourceType::stages, modPath);

            // 收集所有歌曲 JSON
            QVector<json> songJsonContents;
            QVector<QString> songJsonPaths;

            for (auto& songRootPath : songRootPaths)
            {
                std::optional<QStringList> jsonFiles = FileUtil::getFileNames(songRootPath, "*.json");
                if (!jsonFiles.has_value()) continue;

                for (auto& jsonFile : jsonFiles.value())
                {
                    // 跳过 events.json（事件文件不是音符样式）
                    if (jsonFile.endsWith("events.json")) continue;

                    try
                    {
                        json j = json::parse(FileUtil::ReadFileToString(jsonFile).toStdString());
                        songJsonContents.push_back(j);
                        songJsonPaths.push_back(jsonFile);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_JSON_PARSE_ERROR(false, e.what(), jsonFile);
                    }
                }
            }

            // 收集所有舞台 JSON
            QVector<json> stageJsonContents;
            QVector<QString> stageJsonPaths;

            for (auto& stageRootPath : stageRootPaths)
            {
                std::optional<QStringList> jsonFiles = FileUtil::getFileNames(stageRootPath, "*.json");
                if (!jsonFiles.has_value()) continue;

                for (auto& jsonFile : jsonFiles.value())
                {
                    try
                    {
                        json j = json::parse(FileUtil::ReadFileToString(jsonFile).toStdString());
                        stageJsonContents.push_back(j);
                        stageJsonPaths.push_back(jsonFile);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_JSON_PARSE_ERROR(false, e.what(), jsonFile);
                    }
                }
            }

            // 构建歌曲与舞台的配对
            QVector<std::pair<json, json>> noteStageMap = NoteStyleParser::buildNoteStageMap(
                stageJsonContents, songJsonContents, stageJsonPaths);

            // 去重：避免同一箭头皮肤重复解析
            QSet<QString> processedSkins;

            for (auto& [stageJson, songJson] : noteStageMap)
            {
                // 提取箭头皮肤名称
                QString arrowSkin = NoteStyleParser::getNoteSkinsNameOrDefault(songJson, "noteSkins/NOTE_assets");
                QString splashSkin = NoteStyleParser::getNoteSplashNameOrDefault(songJson, "noteSplashes/noteSplashes");

                // 生成唯一标识（箭头皮肤 + 打击特效 + 舞台 UI）
                QString skinKey = arrowSkin + "|" + splashSkin;

                // 如果已经处理过相同组合，跳过避免重复
                if (processedSkins.contains(skinKey))
                    continue;

                processedSkins.insert(skinKey);

                // 解析音符样式
                std::unique_ptr<NoteStyleData> noteStyle = NoteStyleParser::parseNoteStyleData_PE(
                    songJson, stageJson, modPath, "");

                if (noteStyle)
                {
                    // 使用箭头皮肤名作为ID
                    QString styleId = FileUtil::fetchIdFromFileName(arrowSkin);
                    if (styleId.isEmpty())
                        styleId = "psych default";

                    Context::modRegistry->getCache()->noteStyleRelative = noteStyle->toString();
                    Context::noteStyleRegistry->addEntry(styleId, std::move(noteStyle));
                    LOG_INFO("成功解析了PE音符样式: " + styleId);
                }
                else
                {
                    LOG_WARNING(false, "解析PE音符样式失败: " + arrowSkin);
                }
            }

            break;
        }
    default:
        {
            LOG_WARNING(false, "不支持的模组引擎版本");
            return false;
        }
    }
    return true;
}
