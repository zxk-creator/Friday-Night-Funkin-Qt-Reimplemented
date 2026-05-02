//
// Created by kkplay on 4/25/26.
//

#pragma once

#include <string>
#include <optional>

#include "data/song/SongData.h"
#include "play/ISerializable.h"

/**
 * 一首歌就是一个对象，包含了难度，变体等数据
 * 播放路径存储在对应的metadata数据里面了，需要先加载才能播放
 */
class Song : public ISerializable
{
public:

    /**
     * 其实我一直有个疑问，官方为何要把这个搞这么复杂，一首歌一个Song对象不就行了么？为什么一个Song里面还要搞这么多变体之类的东西
     * 可能他有他的好处
     * 谁知道
     * 难崩，花了我三四天才搞明白怎么回事
     */

    static inline const QString DEFAULT_SONGNAME = "Unknown";
    static inline const QString DEFAULT_ARTIST = "Unknown";
    static inline const SongTimeFormat DEFAULT_TIMEFORMAT = SongTimeFormat::MILLISECONDS;
    static inline const std::optional<int> DEFAULT_DIVISIONS = std::nullopt;
    static inline const bool DEFAULT_LOOPED = false;
    static inline const QString DEFAULT_STAGE = "mainStage";
    static inline const float DEFAULT_SCROLLSPEED = 1.0f;

    // 这首歌的ID（default变体的）
    QString id;

    /**
     * {变体id,变体元数据（存储了歌曲的绝对路径！）}
     */
    QHash<QString,SongMetaData> metadatas;   // "default", "bf", "erect", "pico"

    /**
     * {变体id,变体的铺面（包含难度）}
     */
    QHash<QString,SongChartData> notes;

    // 最起码得有个Id吧
    Song(const QString& songId)
    {
        this->id = songId;
    }

    /**
     * @param songId 默认变体的名字
     * @param allMetadata {变体名：元数据}
     * @param songNotes {变体id,变体所有的难度铺面}
     */
    Song(const QString& songId, const QHash<QString,SongMetaData>& allMetadata, const QHash<QString,SongChartData>& songNotes)
    {
        this->id = songId;
        this->metadatas = allMetadata;
        this->notes = songNotes;
    }

    /**
     * 添加歌曲
     * @param variationId 变体名称，至少传个default
     * @param metadata 元数据（包括音频文件绝对路径等）
     * @param songNotes 铺面箭头数据
     */
    void addVariationSong(const QString& variationId, const SongMetaData& metadata,const SongChartData& songNotes)
    {
        metadatas[variationId] = metadata;
        notes[variationId] = songNotes;
    }

    // 把这首歌所有内容都给你弄过去
    QString toString() const override
    {
        QString division = "========== 歌曲详细信息 ==========\n";
        QString res = division;
        res += "歌曲ID: " + id + "\n";

        // 遍历所有变体的元数据
        res += "\n--- 变体元数据 ---\n";
        QHashIterator meta_it(metadatas);
        while (meta_it.hasNext())
        {
            meta_it.next();
            res += "变体: " + meta_it.key() + "\n";
            res += meta_it.value().toString();
            res += "\n";
        }

        // 遍历所有变体的铺面数据
        res += "--- 变体铺面 ---\n";
        QHashIterator chart_it(notes);
        while (chart_it.hasNext())
        {
            chart_it.next();
            res += "变体: " + chart_it.key() + "\n";
            res += chart_it.value().toString();
            res += "\n";
        }

        return res + division;
    }

    QString oneToString(const QString& theId) const override
    {
        return toString();
    }
};
