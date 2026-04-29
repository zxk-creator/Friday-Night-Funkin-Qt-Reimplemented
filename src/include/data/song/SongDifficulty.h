//
// Created by kkplay on 4/28/26.
//

#pragma once
#include <string>

#include "SongData.h"

struct SongDifficulty {
    std::string difficulty;      // "easy", "normal", "hard", "erect", "nightmare"
    std::string variation;       // "default", "bf", "erect", "pico"
    SongMetaData metadata;       // 关联的元数据
    // 谱面数据（按需加载）
    std::optional<SongChartData> chartData;

    // 默认构造函数用于默认拷贝（比如访问某个成员发生拷贝）
    SongDifficulty() = default;

    // 从 metadata 和 difficulty ID 构建
    SongDifficulty(const std::string& diff, const std::string& vari, const SongMetaData& meta)
        : difficulty(diff), variation(vari), metadata(meta) {}
};
