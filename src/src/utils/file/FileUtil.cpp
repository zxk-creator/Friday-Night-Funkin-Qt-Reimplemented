//
// Created by kkplay on 4/24/26.
//

#include "utils/file/FileUtil.h"
#include "utils/exception/CustomException.h"

using namespace std;

// 直接实现，不用 namespace 包装
QString FileUtil::ReadFileToString(const QString& fileAbsolutePath)
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
        Exception::logParseModException(ModParseExcpetionType::OpenFileFail, file.fileName());
        return "";
    }

    QByteArray data = file.readAll();
    QString contents = QString::fromUtf8(data);
    file.close();
    return contents;
}

optional<QStringList> FileUtil::getFileNames(const QString& dirAbsolutePath, const QString& filterStr)
{
    QDir levelDir(dirAbsolutePath);
    if (!levelDir.exists())
    {
        Exception::logParseModException(ModParseExcpetionType::NoFileOrDir, dirAbsolutePath);
        return nullopt;
    }

    QStringList filter;
    filter << filterStr;
    levelDir.setNameFilters(filter);

    return levelDir.entryList(QDir::Files);
}

optional<QStringList> FileUtil::getFileAbsolutePaths(const QString& dirAbsolutePath, const QString& filterStr)
{
    QDir levelDir(dirAbsolutePath);

    if (!levelDir.exists())
    {
        Exception::logParseModException(ModParseExcpetionType::NoFileOrDir, dirAbsolutePath);
        return nullopt;
    }

    QStringList filter;
    filter << filterStr;
    levelDir.setNameFilters(filter);

    QStringList fileNames = levelDir.entryList(QDir::Files);

    QStringList absolutePaths;
    for (const auto& fileName : fileNames)
    {
        absolutePaths << levelDir.absoluteFilePath(fileName);
    }

    return absolutePaths;
}

QString FileUtil::stripMetadataSuffix(const QString& containStr)
{
    QString toRemove = "-metadata";
    QString res = containStr;
    qsizetype index = res.lastIndexOf(toRemove);
    if (index != -1)
        res.remove(index, toRemove.length());

    return res;
}

QString FileUtil::getDefaultSongMetaFilePath(const QString& songPath)
{
    QChar separator = QDir::separator();
    QString songId = songPath.section(separator, -1);
    return songPath + QDir::separator() + songId + "-metadata.json";
}

QString FileUtil::getDefaultSongChartFilePath(const QString& songPath)
{
    QChar separator = QDir::separator();
    QString songId = songPath.section(separator, -1);
    return songPath + QDir::separator() + songId + "-chart.json";
}

QString FileUtil::getVariationSongMetaFilePath(const QString& songPath, const QString& variationId)
{
    QChar separator = QDir::separator();
    QString songId = songPath.section(separator, -1);
    return songPath + QDir::separator() + songId + "-metadata" + "-" + variationId + ".json";
}

QString FileUtil::getVariationSongChartFilePath(const QString& songPath, const QString& variationId)
{
    QChar separator = QDir::separator();
    QString songId = songPath.section(separator, -1);
    return songPath + QDir::separator() + songId + "-chart" + "-" + variationId + ".json";
}

optional<QString> FileUtil::getPathLeaf(const QString& path)
{
    QString res = path.section(QDir::separator(), -1);
    if (res.isEmpty()) return nullopt;

    return res;
}

QString FileUtil::getSongFullPath(const QString& modAbsolutePath, bool isInst,
                                   const QString& playerId, const QString& variationId, const QString& songId)
{
    if (modAbsolutePath.isEmpty()) {
        LOG_WRONG_PARAM_ERROR("传入了空的路径！程序已终止。");
        return "";
    }

    QString songAbsolutePath = Path::getVSSongFilePath(modAbsolutePath, songId);

    QString fileName;
    if (isInst) {
        fileName = variationId.isEmpty() ? "Inst.ogg" : "Inst-" + variationId + ".ogg";
    } else {
        fileName = variationId.isEmpty() ? "Voices-" + playerId + ".ogg" : "Voices-" + playerId + "-" + variationId + ".ogg";
    }

    QString oggPath = songAbsolutePath + QDir::separator() + fileName;
    if (QFile::exists(oggPath))
        return oggPath;

    QString mp3Path = oggPath.replace(".ogg", ".mp3");
    if (QFile::exists(mp3Path))
        return mp3Path;

    Exception::logParseModException(ModParseExcpetionType::NoFileOrDir, oggPath + " 和 " + mp3Path);
    return "";
}

QString FileUtil::fetchIdFromFileName(const QString& fileNameOrPath)
{
    QFileInfo fileInfo(fileNameOrPath);
    return fileInfo.completeBaseName();
}