//
// Created by kkplay on 4/16/26.
//
#include "utils/modding/ModScanUtils.h"
#include "../thirdParty/nlohmann/json.hpp"
#include "utils/InstanceLibrary.h"

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
optional<ModMetadata> ModScanUtils::scanPEMod(QString& modAbsolutePath)
{
    ModMetadata modMetadata{};
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
optional<ModMetadata> ModScanUtils::scanVSMod(QString& modAbsolutePath)
{
    ModMetadata modMetadata{};
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
            QMap<std::string, VersionRules> dependencyMap;
            auto deps = j["optionalDependencies"];
            for (auto it = deps.begin();it != deps.end();++it)
            {
                dependencyMap.insert(it.key(), VersionRules(it.value().get<string>()));
            }
            modMetadata.optionalDependencies = dependencyMap;
        }
        if (j.contains("dependencies") && j["dependencies"].is_object())
        {
            QMap<std::string, VersionRules> dependencyMap;
            auto deps = j["dependencies"];
            for (auto it = deps.begin();it != deps.end();++it)
            {
                dependencyMap.insert(it.key(), VersionRules(it.value().get<string>()));
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