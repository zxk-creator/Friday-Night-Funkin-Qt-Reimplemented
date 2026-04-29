//
// Created by kkplay on 4/16/26.
//
#include "utils/modding/ModScanUtils.h"
#include "../thirdParty/nlohmann/json.hpp"
#include "utils/InstanceLibrary.h"
#include "utils/exception/CustomException.h"
#include "data/level/LevelData.h"
#include "utils/VersionUtil.h"
#include "data/level/LevelRegistry.h"
#include "data/song/SongData.h"
#include "src/src/utils/file/FileUtil.h"

using Lang = InstanceLibrary;

ModEngineType ModScanUtils::judgeModEngine(const QString& modAbsolutePath)
{
    QDir modDir(modAbsolutePath);
    QFileInfo modInfo(modDir.absolutePath());
    qInfo() << "传入的路径是: " << modDir.absolutePath();
    QString PECoreCfg = "pack.json";
    QString VSCoreCfg = "_polymod_meta.json";
    if (QFile::exists(modDir.filePath(PECoreCfg)))
    {
        qInfo() << modInfo.fileName() << "是PE模组！";
        return ModEngineType::PE;
    }
    if (QFile::exists(modDir.filePath(VSCoreCfg)))
    {
        qInfo() << modInfo.fileName() << "是V-Slice模组！";
        return ModEngineType::VS;
    }

    qInfo() << modInfo.fileName() << "没找到合适的配置文件！因此我默认他为PE。";
    return ModEngineType::PE;
}

// 真正意义上解析模组！
// TODO: Linux上大小写敏感，icon.png和icon.PNG完全是两个东西。等出问题了再修吧:)
optional<ModMetadata> ModScanUtils::scanPEModMetadata(QString& modAbsolutePath)
{
    ModMetadata modMetadata{};
    modMetadata.modPath = modAbsolutePath.toStdString();
    modMetadata.engineType = ModEngineType::PE;

    QDir modDir(modAbsolutePath);
    QString cfgPath = modDir.filePath("pack.json");
    if (!QFile::exists(cfgPath))
    {
        qWarning() << "PE模组" << modAbsolutePath << "没有pack.json!不过我仍旧尝试解析他...";
        return modMetadata;
    }

    QFile cfgFile(cfgPath);
    QFileInfo dirName(modDir.absolutePath());
    if (!cfgFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "无法解析PE模组" << dirName.fileName() << "的pack.json文件，不过我仍旧尝试解析他";
        return modMetadata;
    }

    try {
        // 真正解析模组
        string rawJSON = cfgFile.readAll().toStdString();
        // 忽略注释和多余逗号
        auto j = nlohmann::json::parse(rawJSON,nullptr,true,true);
        modMetadata.title = j.value("name", Lang::langStringPool->untitledMod().toStdString());
        modMetadata.description = j.value("description", Lang::langStringPool->noDescription().toStdString());
        modMetadata.restart = j.value("restart", false);
        modMetadata.runsGlobally = j.value("runsGlobally", false);
        modMetadata.bgRGBColor = j.value<optional<vector<int>>>("color",nullopt);
        QString normalIconPath = modDir.filePath("pack.png");
        QString pixelIconPath = modDir.filePath("pack-pixel.png");
        QFileInfo normalIconInfo(normalIconPath);
        QFileInfo pixelIconInfo(pixelIconPath);
        if (normalIconInfo.exists())
            modMetadata.iconPath = normalIconPath.toStdString();
        else if (pixelIconInfo.exists())
            modMetadata.iconPath = pixelIconPath.toStdString();
        else
            modMetadata.iconPath = nullopt;

        qInfo() << modMetadata.toString();
        return modMetadata;
    }
    catch (const std::exception& e)
    {
        qCritical() << "解析json时发生异常：" << e.what();
        return nullopt;
    }
}

// TODO: Linux上大小写敏感，icon.png和icon.PNG完全是两个东西。等出问题了再修吧:)
optional<ModMetadata> ModScanUtils::scanVSModMetadata(QString& modAbsolutePath)
{
    ModMetadata modMetadata{};
    modMetadata.modPath = modAbsolutePath.toStdString();
    modMetadata.engineType = ModEngineType::VS;

    QDir modDir(modAbsolutePath);
    QString cfgPath = modDir.filePath("_polymod_meta.json");
    QString iconPath = modDir.filePath("_polymod_icon.png");
    if (!QFile::exists(cfgPath))
    {
        qWarning() << "V-Slice模组" << modAbsolutePath << "没有_polymod_meta.json！此模组无效。";
        return nullopt;
    }

    QFile cfgFile(cfgPath);
    QFileInfo dirName(modDir.absolutePath());
    QFileInfo iconInfo(iconPath);
    if (!cfgFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "无法解析V-Slice模组" << dirName.fileName() << "的_polymod_meta.json文件！此模组无效。";
        return nullopt;
    }

    try {
        string rawJSON = cfgFile.readAll().toStdString();
        auto j = nlohmann::json::parse(rawJSON,nullptr,true,true);
        modMetadata.title = j.value("title",Lang::langStringPool->untitledMod().toStdString());
        modMetadata.description = j.value("description",Lang::langStringPool->noDescription().toStdString());
        modMetadata.homepage = j.value<optional<string>>("homepage",nullopt);
        modMetadata.apiVersion = j.value<optional<string>>("api_version",nullopt);
        if (modMetadata.apiVersion.has_value())
        {
            modMetadata.apiVersion.value() = "V-Slice " + modMetadata.apiVersion.value();
        }
        modMetadata.modVersion = j.value<optional<string>>("mod_version",nullopt);
        modMetadata.license = j.value<optional<string>>("license",nullopt);

        // 处理模组图标
        if (iconInfo.exists())
        {
            qInfo() << dirName.fileName() << "有图标!";
            modMetadata.iconPath = iconPath.toStdString();
        }
        else
        {
            qInfo() << dirName.fileName() << "没有图标！ 当前他为" << modMetadata.iconPath.has_value();
        }

        // 处理dependencies和optionalDependencies
        if (j.contains("optionalDependencies") && j["optionalDependencies"].is_object())
        {
            QMap<std::string, string> dependencyMap;
            auto deps = j["optionalDependencies"];
            for (auto it = deps.begin();it != deps.end();++it)
            {
                dependencyMap.insert(it.key(), it.value().get<string>());
            }
            modMetadata.optionalDependencies = dependencyMap;
        }
        if (j.contains("dependencies") && j["dependencies"].is_object())
        {
            QMap<std::string, string> dependencyMap;
            auto deps = j["dependencies"];
            for (auto it = deps.begin();it != deps.end();++it)
            {
                dependencyMap.insert(it.key(), it.value().get<string>());
            }
            modMetadata.dependencies = dependencyMap;
        }

        // 处理嵌套的contributor数组，拼接成完整字符串！
        if (j.contains("contributors") && j["contributors"].is_array())
        {
            // 防止nullopt导致崩溃
            string currentStr = modMetadata.contributors.value_or("");

            // 直接使用html进行类似网页设计！
            for (const auto& item : j["contributors"]) {
                string name = item.value("name", "Unknown");
                string url = item.value("url", "");
                string entry;
                if (item.contains("roles"))
                {
                    string roles = item.value("roles", "");
                    entry =
                    "<p style='margin-bottom: 10px;'>"
                    "<b><font size='1' color='#FFFFFF'>" + name + "</font></b><br/>" +
                    "<font color='#AAAAAA'>" + roles + "</font><br/>";
                }
                else if (item.contains("role"))
                {
                    string role = item.value("role", "");
                    entry =
                    "<p style='margin-bottom: 10px;'>"
                    "<b><font size='1' color='#FFFFFF'>" + name + "</font></b><br/>" +
                    "<font color='#AAAAAA'>" + role + "</font><br/>";
                }

                if (!url.empty()) {
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
        qCritical() << "解析json时发生异常：" << e.what();
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
            qWarning() << "解析 " << QString::fromStdString(entry.modPath) << "的levels的时候出现问题\n已跳过";

        parseSongs(entry);
        parseCharacters(entry);
        parseStages(entry);
    }
}

bool ModScanUtils::parseWeeks(const ModMetadata& modMetadata)
{
    string modAbsolutePath = modMetadata.modPath;
    QString finalLevelDirPath = QDir::cleanPath(QString::fromStdString(modAbsolutePath) + QDir::separator() + "data" + QDir::separator() + "levels");

    optional<QStringList> jsonFiles = FileUtil::validateAndGetFileNamesInDir(finalLevelDirPath,"*.json");
    if (!jsonFiles.has_value()) return false;

    foreach(const QString& fileName, jsonFiles.value())
    {
        QString oneJSONFile = finalLevelDirPath + QDir::separator() + fileName;
        // 直接开始解析json
        QString content = FileUtil::ReadFileToString(oneJSONFile);

        // 由于我们已经写好了from_json函数，因此可以直接获取！
        json j = nlohmann::json::parse(content.toStdString());
        LevelData levelData;
        LevelData_from_json_VS(j,levelData);
        LevelRegistry::addAvailableWeeks(levelData,QString::fromStdString(modAbsolutePath));
    }

    return true;
}

bool ModScanUtils::parseCharacters(const ModMetadata& modMetadata)
{

}

bool ModScanUtils::parseSongs(const ModMetadata& modMetadata)
{
    QString modAbsolutePath = QString::fromStdString(modMetadata.modPath);
    // 接下来我们就得去songs目录下去找json了
    QString songsRootPath = QDir::cleanPath(modAbsolutePath + QDir::separator() + "data" + QDir::separator() + "songs");
    QDir dir(songsRootPath);
    QStringList songsSubFolders = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString& songIdAndFolderName : songsSubFolders) {
        QString oneSongFolderPath = songsRootPath + QDir::separator() + songIdAndFolderName;

        optional<QStringList> songMetaFileNames = FileUtil::validateAndGetFileNamesInDir(oneSongFolderPath,"*-metadata.json");
        if (!songMetaFileNames.has_value()) continue;

        /**
         * 这遍历的是元数据文件,先遍历元数据，再按需加载歌曲
         */
        foreach(const QString& filename,songMetaFileNames.value())
        {
            /**
              * 获得歌曲文件夹名字后，进入才能看到铺面和metadata。
              * 这里需要注意的是: 有些歌比如erect和nightmare难度是不会出现在storymenu里面的
              * 什么意思？data/song目录下的每个json，并不是每个都出现在storymenu里面的
              * 所以我们需要分别注册level，把歌注册到level对象里面一个level对象只包含他在json里面声明的那几首歌
              * 但是为那些隐藏的也得new Song对象出来，方便后续在freeplay里面显示
              * 模组详情界面也要显示！即便是他隐藏了
              * 注：erect和nightmare，pico是变体，需要特殊处理，根据源码，他不和storymenu（LEVEL对象）里面的简单普通困难一块出现。
              * erect都有自己的专属铺面，使用方式为 原铺面-erect.json
              * 还有可玩角色-pico.json，也是一种变体，和bf，erect同级，只出现在freeplay里面
              * 经过研究：storymenu只有简单普通困难三个选项，erect和nightmare需要单独声明
              * 当选择的角色是pico或pico-playable的时候，才会出现在pico的freeplay里面。
              * 当歌曲difficulties数组支持erect和nightmare的时候，才会出现在erect和nightmare选项里面。
              * 关键字段：
              * "songVariations": ["bf", "erect"]
              * "difficulties": ["easy", "normal", "hard"]
              * "player": "pico-playable"
              * 这就是一个pico专属歌，难度有erect，变体是bf，具体定义在"歌曲名-bf.json中"
              * 不带后缀的都是普通变体，优先加载的是他们。
             */
            // 总之，我们要根据这个获得Song对象！
            // 另外，Metadata对象存储在每一个Song对象里面！
            // 首先解析的是metadata文件！
            QString thisJsonFilePath = oneSongFolderPath + QDir::separator() + filename;
            try {
                json j = json::parse(FileUtil::ReadFileToString(thisJsonFilePath.toStdString()));
                optional<SongMetaData> _metadata = SongDataParser::parseSongMetaData(j,filename.toStdString());


            }
            catch (const json::parse_error& e)
            {
                LOG_JSON_PARSE_ERROR(e.what(),filename);
                continue;
            }
        }
    }
}

bool ModScanUtils::parseStages(const ModMetadata& modMetadata)
{

}

bool ModScanUtils::parseNotestyles(const ModMetadata& modMetadata)
{

}
