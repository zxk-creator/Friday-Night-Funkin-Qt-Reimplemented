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

    MessageHandler::logWarning(modInfo.fileName() + "没找到合适的配置文件！因此我默认他为PE。", "ModScanUtils");
    return ModEngineType::PE;
}

// 真正意义上解析模组！
// TODO: Linux上大小写敏感，icon.png和icon.PNG完全是两个东西。等出问题了再修吧:)
void ModScanUtils::scanPEModMetadata(const QString& modAbsolutePath,ModRegistry* registryRef)
{
    auto modMetadata = make_unique<ModMetadata>();
    modMetadata->modPath = modAbsolutePath;
    modMetadata->engineType = ModEngineType::PE;

    QDir modDir(modAbsolutePath);
    QString cfgPath = modDir.filePath("pack.json");
    if (!QFile::exists(cfgPath))
        MessageHandler::logWarning(modAbsolutePath + "没有pack.json!不过我仍旧尝试解析他...", "ModScanUtils");


    QFile cfgFile(cfgPath);
    QFileInfo dirName(modDir.absolutePath());
    if (!cfgFile.open(QIODevice::ReadOnly | QIODevice::Text))
        MessageHandler::logWarning(dirName.fileName() + "无法解析pack.json文件，不过我仍旧尝试解析他", "ModScanUtils");

    try {
        // 真正解析模组
        string rawJSON = cfgFile.readAll().toStdString();
        // 忽略注释和多余逗号
        auto j = nlohmann::json::parse(rawJSON, nullptr, true, true);

        modMetadata->title = QString::fromStdString(j.value("name", LangStringPool::instance()->untitledMod().toStdString()));
        modMetadata->description = QString::fromStdString(j.value("description", LangStringPool::instance()->noDescription().toStdString()));
        modMetadata->restart = j.value("restart", false);
        modMetadata->runsGlobally = j.value("runsGlobally", false);

        optional<QString> id = FileUtil::getPathLeaf(modAbsolutePath);
        if (!id.has_value()) id = "Unnamed";
        modMetadata->id = id;

        if (j.contains("color") && j["color"].is_array()) {
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
        registryRef->addEntry(id.value(),std::move(modMetadata));
    }
    catch (const std::exception& e)
    {
        MessageHandler::logError(QString("解析" + modAbsolutePath + "时发生异常：") + e.what() + "已跳过。" + "\n", "ModScanUtils");
    }
}

// TODO: Linux上大小写敏感，icon.png和icon.PNG完全是两个东西。等出问题了再修吧:)
void ModScanUtils::scanVSModMetadata(const QString& modAbsolutePath,ModRegistry* registryRef)
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
        MessageHandler::logWarning(modAbsolutePath + "没有_polymod_meta.json！此模组无效。", "ModScanUtils");
    }

    QFile cfgFile(cfgPath);
    QFileInfo dirName(modDir.absolutePath());
    QFileInfo iconInfo(iconPath);
    if (!cfgFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        MessageHandler::logWarning(dirName.fileName() + "无法解析_polymod_meta.json文件！此模组无效。", "ModScanUtils");
    }

    try {
        string rawJSON = cfgFile.readAll().toStdString();
        auto j = nlohmann::json::parse(rawJSON, nullptr, true, true);

        modMetadata->title = QString::fromStdString(j.value("title", LangStringPool::instance()->untitledMod().toStdString()));
        modMetadata->description = QString::fromStdString(j.value("description", LangStringPool::instance()->noDescription().toStdString()));

        if (j.contains("homepage") && j["homepage"].is_string()) {
            modMetadata->homepage = QString::fromStdString(j["homepage"].get<string>());
        }

        if (j.contains("api_version") && j["api_version"].is_string()) {
            QString apiVer = QString::fromStdString(j["api_version"].get<string>());
            modMetadata->apiVersion = "V-Slice " + apiVer;
        }

        if (j.contains("mod_version") && j["mod_version"].is_string()) {
            modMetadata->modVersion = QString::fromStdString(j["mod_version"].get<string>());
        }

        if (j.contains("license") && j["license"].is_string()) {
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
            for (auto& [key, val] : j["dependencies"].items()) {
                dependencyMap.insert(QString::fromStdString(key), QString::fromStdString(val.get<string>()));
            }
            modMetadata->dependencies = dependencyMap;
        }

        if (j.contains("optionalDependencies") && j["optionalDependencies"].is_object())
        {
            ModDependencies dependencyMap;
            for (auto& [key, val] : j["optionalDependencies"].items()) {
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
            modMetadata->contributors = currentStr;
        }

        // qInfo() << modMetadata->toString();
        // 注册！
        registryRef->addEntry(id.value(),std::move(modMetadata));
        LOG_INFO("成功解析了模组元数据: " + id.value());
    }
    catch (const std::exception& e){
        MessageHandler::logError(QString("解析" + modAbsolutePath + "时发生异常：") + e.what() + "已跳过。" + "\n", "ModScanUtils");
    }
}

void ModScanUtils::scanAllMods(const QVector<ModMetadata>& modMetadatas)
{
    for (auto& entry : modMetadatas)
    {
        string levelName;
        // 按顺序来，不能错！
        if (parseWeeks(entry))
            MessageHandler::logWarning("解析" + entry.modPath + "时出现问题，已跳过。", "ModScanUtils");

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
    switch (modMetadata.engineType)
    {
    case ModEngineType::VS: {
            QString finalLevelDirPath = Path::getVSDataPath(EDataResourceType::levels,modAbsolutePath);

            optional<QStringList> jsonFiles = FileUtil::getFileAbsolutePaths(finalLevelDirPath,"*.json");
            if (!jsonFiles.has_value()) return false;

            for(auto& filePath : jsonFiles.value())
            {
                // 直接开始解析json
                QString content = FileUtil::ReadFileToString(filePath);
                QString id = FileUtil::fetchIdFromFileName(filePath);

                // 由于我们已经写好了from_json函数，因此可以直接获取！
                json j = nlohmann::json::parse(content.toStdString());
                auto levelData = LevelDataParser::parseLevelData_VS(j,filePath);
                if (!levelData) continue;

                Context::levelRegistry->addEntry(id,std::move(levelData));
                LOG_INFO("成功解析了关卡: " + filePath);
            }
            break;
        }
    case ModEngineType::PE:
        {
            QVector<QString> PELevelDirPath = Path::getPEDataPath(EDataResourceType::levels,modAbsolutePath);
            for (auto& entry : PELevelDirPath)
            {
                optional<QStringList> jsonFiles = FileUtil::getFileAbsolutePaths(entry,"*.json");
                if (!jsonFiles.has_value()) continue;

                // 然后获取里面所有的json文件
                for (auto& jsonFile : jsonFiles.value())
                {
                    QString rawContent = FileUtil::ReadFileToString(jsonFile);
                    try
                    {
                        json j = nlohmann::json::parse(rawContent.toStdString());
                        unique_ptr<LevelData> levelData = LevelDataParser::parseLevelData_PE(j,jsonFile);
                        if (!levelData) continue;

                        QString idnum = FileUtil::genUID();
                        QString id = FileUtil::fetchIdFromFileName(jsonFile) + idnum;
                        // PE要id其实没啥用
                        Context::levelRegistry->addEntry(id,std::move(levelData));
                        LOG_PARSE_JSON_SUCCESS("Week",jsonFile);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_JSON_PARSE_ERROR(e.what(),jsonFile);
                    }
                }
            }

            break;
        }
        default:
        {
            LOG_WARNING("暂不支持的模组类型。");
            return false;
        }
    }

    return true;
}

bool ModScanUtils::parseSongs(const ModMetadata& modMetadata)
{
    switch (modMetadata.engineType) {
        case ModEngineType::VS: {
            QString modAbsolutePath = modMetadata.modPath;
            // 接下来我们就得去songs目录下去找json了
            QString songsRootPath = Path::getVSDataPath(EDataResourceType::songs, modAbsolutePath);
            QDir dir(songsRootPath);
            QStringList songsSubFolders = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

            for (const QString& songIdAndFolderName : songsSubFolders) {
                QString thisSongDataFolderPath = songsRootPath + QDir::separator() + songIdAndFolderName;
                QString defaultSongMetaFilePath = FileUtil::getDefaultSongMetaFilePath(thisSongDataFolderPath);

                optional<QString> thisMetaFileName = FileUtil::getPathLeaf(defaultSongMetaFilePath);
                if (!thisMetaFileName.has_value())  continue;

                QString thisChartFilePath = FileUtil::getDefaultSongChartFilePath(thisSongDataFolderPath);
                try {
                    // 先解析普通元数据文件
                    json jMeta = json::parse(FileUtil::ReadFileToString(defaultSongMetaFilePath.toStdString()));
                    optional<SongMetaData> defaultMetadata = SongDataParser::parseSongMetaData(jMeta,thisMetaFileName.value());
                    // 先解析普通铺面文件
                    json jChart = json::parse(FileUtil::ReadFileToString(thisChartFilePath).toStdString());
                    SongChartData defaultChartData = SongDataParser::parseSongChartData(jChart,thisMetaFileName.value());

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
                            MessageHandler::logError(" 异常已发生: "+ QString(e.what()) + "在" + songIdAndFolderName, "ModScanUtils::parseSongs");
                            continue;
                        }
                    }

                    Context::levelRegistry->addEntry(songIdAndFolderName,std::move(song));
                    // 至此，我们终于完成了一首歌的构造！🎉🎉🎉🎉
                    LOG_INFO("成功解析了歌曲铺面和元数据: " + songIdAndFolderName);
                }
                catch (const json::parse_error& e)
                {
                    LOG_JSON_PARSE_ERROR(e.what(),songIdAndFolderName);
                    continue;
                }
            }
        }
    case ModEngineType::PE:
            {

            }
        default:
            {
                LOG_WARNING("暂不支持的模组类型。");
            }
    }

    return true;
}

bool ModScanUtils::parseCharacters(const ModMetadata& modMetadata)
{
    QString modAbsolutePath = modMetadata.modPath;
    // 思路：找到data/character目录下的所有json文件并递归解析
    QString characterDataPath = Path::getVSDataPath(EDataResourceType::character,modAbsolutePath);
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
            Context::characterRegistry->addEntry(id,std::move(cd));
            LOG_INFO("成功解析了角色: " + id);
        }
        catch (const json::parse_error& e)
        {
            LOG_JSON_PARSE_ERROR(e.what(),file);
            continue;
        }
    }

    return true;
}

bool ModScanUtils::parseStages(const ModMetadata& modMetadata)
{
    QString modPath = modMetadata.modPath;
    QString modStageJsonPath = Path::getVSDataPath(EDataResourceType::stages,modPath);

    optional<QStringList> fileList = FileUtil::getFileAbsolutePaths(modStageJsonPath,".json");
    if (!fileList.has_value()) return false;
    for (auto& filepath : fileList.value())
    {
        try
        {
            QString rawJSON = FileUtil::ReadFileToString(filepath);
            json j = json::parse(rawJSON.toStdString());
            unique_ptr<StageData> stageData = StageDataParser::parseStageData(j,filepath);
            if (!stageData) continue;
            QString id = FileUtil::fetchIdFromFileName(filepath);
            Context::stageRegistry->addEntry(id,std::move(stageData));

            LOG_INFO("成功解析了StageData: " + id);

        } catch (const json::parse_error& e) {
            LOG_ERROR(QString("解析JSON" + filepath + "时发生异常: ") + e.what() + "已跳过。");
        }
    }

    return true;
}

bool ModScanUtils::parseNotestyles(const ModMetadata& modMetadata)
{
    QString modPath = modMetadata.modPath;
    QString modStageJsonPath = Path::getVSDataPath(EDataResourceType::notestyles,modPath);

    optional<QStringList> fileList = FileUtil::getFileAbsolutePaths(modStageJsonPath,".json");
    if (!fileList.has_value()) return false;
    for (auto& filepath : fileList.value())
    {
        try
        {
            QString rawJSON = FileUtil::ReadFileToString(filepath);
            json j = json::parse(rawJSON.toStdString());
            unique_ptr<NoteStyleData> noteStyleData = NoteStyleParser::parseNoteStyleData(j,filepath);
            if (!noteStyleData) continue;
            QString id = FileUtil::fetchIdFromFileName(filepath);
            Context::noteStyleRegistry->addEntry(id,std::move(noteStyleData));

            LOG_INFO("成功解析了StageData: " + id);

        } catch (const json::parse_error& e) {
            LOG_ERROR(QString("解析JSON" + filepath + "时发生异常: ") + e.what() + "已跳过。");
        }
    }
}