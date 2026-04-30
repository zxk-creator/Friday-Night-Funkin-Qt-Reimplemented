//
// Created by kkplay on 4/25/26.
//

#pragma once

#include <string>
#include <optional>

#include "data/song/SongData.h"

/**
 * 一首歌就是一个对象，包含了难度，变体等数据
 */
class Song
{
public:

    /**
     * 其实我一直有个疑问，官方为何要把这个搞这么复杂，一首歌一个Song对象不就行了么？为什么一个Song里面还要搞这么多变体之类的东西
     * 可能他有他的好处
     * 谁知道
     * 难崩，花了我三四天才搞明白怎么回事
     */

    static inline const std::string DEFAULT_SONGNAME = "Unknown";
    static inline const std::string DEFAULT_ARTIST = "Unknown";
    static inline const SongTimeFormat DEFAULT_TIMEFORMAT = SongTimeFormat::MILLISECONDS;
    static inline const std::optional<int> DEFAULT_DIVISIONS = std::nullopt;
    static inline const bool DEFAULT_LOOPED = false;
    static inline const std::string DEFAULT_STAGE = "mainStage";
    static inline const float DEFAULT_SCROLLSPEED = 1.0f;

    // 这首歌的ID（default变体的）
    std::string id;

    /**
     * {变体id,变体元数据}
     */
    std::unordered_map<std::string, SongMetaData> metadatas;   // "default", "bf", "erect", "pico"
    /**
     * {变体id,变体的铺面（包含难度）}
     */
    std::unordered_map<std::string, SongChartData> notes;

    /**
     * tmd官方引擎写的代码我是真看不懂啥意思VSCode还没有高亮提示AI给的一团糟所以我不管他怎么设计了我直接按自己想的来qnmd
     * unordered_map的参数 Key: 变体 ID (Variation ID)
     * @param songId 默认变体的名字
     * @param allMetadata {变体名：元数据}
     * @param songNotes {变体id,变体所有的难度铺面}
     */
    Song(const std::string& songId, const std::unordered_map<std::string, SongMetaData>& allMetadata,std::unordered_map<std::string, SongChartData>& songNotes)
    {
        this->id = songId;
        this->metadatas = allMetadata;
        this->notes = songNotes;
    }

    //
};
