//
// Created by kkplay on 4/24/26.
//

#pragma once
#include <QDir>
#include <QFile>
#include <qiodevice.h>
#include <QString>

#include "utils/exception/CustomException.h"

namespace FileUtil
{

    /**
     * 一个函数打开文件
     * @param fileAbsolutePath 文件绝对路径，一定要是文件路径！
     * @return 转换后的字符串
     */
    inline QString ReadFileToString(const QString& fileAbsolutePath)
    {
        if (fileAbsolutePath.isEmpty()) {
            qWarning("传入了空路径！在ReadFileToString函数");
            return "";
        }
        QFile file(fileAbsolutePath);
        if (!file.exists())
        {
            Exception::logParseModException(ModParseExcpetionType::NoFileOrDir, fileAbsolutePath);
            return "";
        }
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            Exception::logParseModException(ModParseExcpetionType::OpenFileFail,file.fileName());
            return "";
        }

        QByteArray data = file.readAll();
        QString contents = QString::fromUtf8(data);
        file.close();
        return contents;
    }

    // 重载版本
    inline string ReadFileToString(const string& fileAbsolutePathStd)
    {
        QString fileAbsolutePath = QString::fromStdString(fileAbsolutePathStd);

        if (fileAbsolutePath.isEmpty()) {
            qWarning("传入了空路径！在ReadFileToString函数");
            return "";
        }
        QFile file(fileAbsolutePath);
        if (!file.exists())
        {
            Exception::logParseModException(ModParseExcpetionType::NoFileOrDir, fileAbsolutePath);
            return "";
        }
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            Exception::logParseModException(ModParseExcpetionType::OpenFileFail,file.fileName());
            return "";
        }

        QByteArray data = file.readAll();
        QString contents = QString::fromUtf8(data);
        file.close();
        return contents.toStdString();
    }

    inline optional<QStringList> validateAndGetFileNamesInDir(const QString& dirAbsolutePath,const QString& filterStr)
    {
        QDir levelDir = dirAbsolutePath;
        if (!levelDir.exists())
        {
            Exception::logParseModException(ModParseExcpetionType::NoFileOrDir,dirAbsolutePath);
            return nullopt;
        }
        QStringList filter;
        filter << filterStr;
        levelDir.setNameFilters(filter);

        return levelDir.entryList(QDir::Files);
    }

    // 去掉metadata后缀以获取歌曲id
    inline QString stripMetadataSuffix(const QString& containStr)
    {
        QString toRemove = "-metadata";
        QString res = containStr;
        qsizetype index = res.lastIndexOf(toRemove);
        if (index != -1) res.remove(index,toRemove.length());

        return res;
    }

    /**
     *
     * @param songPath 包含metadata的绝对路径
     * @return 构造好的默认变体元数据的绝对路径
     * eg: sky-metadata.json
     */
    inline QString getDefaultSongMetaFilePath(const QString& songPath)
    {
        QChar separator = QDir::separator();
        QString songId = songPath.section(separator,-1);
        return songPath + QDir::separator() + songId + "-metadata.json";
    }

    /**
     *
     * @param songPath 包含chart的绝对路径
     * @return 构造好的默认变体铺面的绝对路径
     * eg: sky-chart.json
     */
    inline QString getDefaultSongChartFilePath(const QString& songPath)
    {
        QChar separator = QDir::separator();
        QString songId = songPath.section(separator,-1);
        return songPath + QDir::separator() + songId + "-chart.json";
    }

    /**
     *
     * @param songPath 包含metadata的绝对路径
     * @return 构造好的变体元数据的绝对路径
     * eg: sky-metadata-pico.json
     */
    inline QString getVariationSongMetaFilePath(const QString& songPath, const QString& variationId)
    {
        QChar separator = QDir::separator();
        QString songId = songPath.section(separator,-1);
        return songPath + QDir::separator() + songId + "-metadata" + "-" + variationId + ".json";
    }

    /**
     *
     * @param songPath 包含变体chart的绝对路径
     * @return 构造好的变体元数据的绝对路径
     * eg: sky-chart-pico.json
     */
    inline QString getVariationSongChartFilePath(const QString& songPath, const QString& variationId)
    {
        QChar separator = QDir::separator();
        QString songId = songPath.section(separator,-1);
        return songPath + QDir::separator() + songId + "-chart" + "-" + variationId + ".json";
    }

    /**
     * / 获取例如/home/kkplay/MyCode/cplusplus的cplusplus
     * @param path 路径
     * @return cplusplus
     */
    inline QString getPathLeaf(const QString& path)
    {
        return path.section(QDir::separator(),-1);
    }
}
