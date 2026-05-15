//
// Created by kkplay on 4/24/26.
//

#pragma once

#include <qiodevice.h>
#include <optional>
#include <random>
#include <string>

#include "utils/Path.h"

namespace FileUtil
{
    /**
     * 一个函数打开文件
     * @param fileAbsolutePath 文件绝对路径，一定要是文件路径！
     * @return 转换后的字符串
     */
    QString ReadFileToString(const QString& fileAbsolutePath);

    /**
     * 重载版本，支持std::string
     */
    inline std::string ReadFileToString(const std::string& fileAbsolutePath)
    {
        return ReadFileToString(QString::fromStdString(fileAbsolutePath)).toStdString();
    }

    /**
     * 获得某个目录下的所有文件名（例如: ["file1.txt", "file2.txt"]）
     * @param dirAbsolutePath 文件夹绝对路径
     * @param filterStr 后缀名如 "*.json"
     * @return 包含所有文件名的QStringList
     */
    std::optional<QStringList> getFileNames(const QString& dirAbsolutePath, const QString& filterStr);

    /**
     * 获得某个目录下的所有文件的绝对路径列表
     * @param dirAbsolutePath 文件夹绝对路径
     * @param filterStr 后缀名如 "*.json"
     * @return 包含所有文件绝对路径的QStringList
     */
    std::optional<QStringList> getFileAbsolutePaths(const QString& dirAbsolutePath, const QString& filterStr);

    // 去掉metadata后缀以获取歌曲id
    QString stripMetadataSuffix(const QString& containStr);

    QString getDifficultyStrFromFileName(const QString& fileNameOrPath);

    /**
     * 构造默认变体元数据的绝对路径
     * @param songPath 包含metadata的绝对路径
     * @return 构造好的默认变体元数据的绝对路径
     * eg: sky-metadata.json
     */
    QString getDefaultSongMetaFilePath(const QString& songPath);

    /**
     * 构造默认变体铺面的绝对路径
     * @param songPath 包含chart的绝对路径
     * @return 构造好的默认变体铺面的绝对路径
     * eg: sky-chart.json
     */
    QString getDefaultSongChartFilePath(const QString& songPath);

    /**
     * 构造变体元数据的绝对路径
     * @param songPath 包含metadata的绝对路径
     * @param variationId 变体ID
     * @return 构造好的变体元数据的绝对路径
     * eg: sky-metadata-pico.json
     */
    QString getVariationSongMetaFilePath(const QString& songPath, const QString& variationId);

    /**
     * 构造变体chart的绝对路径
     * @param songPath 包含变体chart的绝对路径
     * @param variationId 变体ID
     * @return 构造好的变体元数据的绝对路径
     * eg: sky-chart-pico.json
     */
    QString getVariationSongChartFilePath(const QString& songPath, const QString& variationId);

    /**
     * 获取路径的最后一个部分
     * @param path 路径
     * @return 路径的叶子节点名称
     * eg: /home/kkplay/MyCode/cplusplus -> cplusplus
     *     /home/kkplay/MyCode.txt       -> MyCode.txt
     */
    std::optional<QString> getPathLeaf(const QString& path);

    /**
     * 获得歌曲的绝对路径（通过字符串拼接）
     * @param modAbsolutePath 模组根目录
     * @param isInst true: 返回Inst.ogg, false: Voice-xx.ogg
     * @param playerId 角色Id
     * @param variationId 变体id
     * @param songId 歌曲Id
     * @return 构建好的路径（找不到返回空字符串）
     */
    QString getSongFullPath(const QString& modAbsolutePath, bool isInst,
                           const QString& playerId, const QString& variationId, const QString& songId);

    /**
     * 从路径或者直接文件名中获取id
     * eg: - "bf-candy.json" → "bf-candy"
     *     - "home/kkplay/mod/files/bf-candy.json" → "bf-candy"
     *     - "home/kkplay/mod/files/songs/wife-forever" → "wife-forever"（这里是文件夹！）
     * @param fileNameOrPath 文件名或路径名
     * @return id
     */
    QString fetchIdFromFileName(const QString& fileNameOrPath);

    /**
     * 转换文件名全部为小写或大写（Linux系统上大小写不同的文件是不同文件）
     * @param toLower 是否转换成小写（反之转换成大写）
     * @param fileName 文件名（只能是文件名，传入路径名在linux上很容易出问题。）
     * @return 转换后的名字
     */
    inline QString toLowerOrToUpper(bool toLower,const QString& fileName)
    {
        if (toLower) return fileName.toLower();
        return fileName.toUpper();
    }

    // 通过当前时间生成UUID（肯定不会重复，除非米哈游有100亿以上的玩家，而且你装了一万个PE模组）
    inline QString genUID()
    {
        static std::atomic<uint64_t> counter{0};
        uint64_t seq = counter.fetch_add(1, std::memory_order_relaxed);

        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()
        ).count();

        return QString("%1%2")
            .arg(timestamp % 100000000, 8, 10, QChar('0'))
            .arg(seq % 10000, 4, 10, QChar('0'));
    }
}
