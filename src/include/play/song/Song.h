//
// Created by kkplay on 4/25/26.
//

#pragma once

#include <string>
#include <optional>
#include <set>

#include "data/song/SongData.h"
#include "data/song/SongDifficulty.h"


class Song
{
public:
    static inline const std::string DEFAULT_SONGNAME = "Unknown";
    static inline const std::string DEFAULT_ARTIST = "Unknown";
    static inline const SongTimeFormat DEFAULT_TIMEFORMAT = SongTimeFormat::MILLISECONDS;
    static inline const std::optional<int> DEFAULT_DIVISIONS = std::nullopt;
    static inline const bool DEFAULT_LOOPED = false;
    static inline const std::string DEFAULT_STAGE = "mainStage";
    static inline const float DEFAULT_SCROLLSPEED = 1.0f;

    // 歌曲 ID
    std::string id;

    /**
     * 关键数据结构，对应了变体及其自身的元数据
     * key: 变体ID 如 "default", "erect", "pico", "bf"
     */
    std::map<std::string, SongMetaData> metadataMap;   // "default", "bf", "erect", "pico"
    /**
     * 所有难度的数据{variation(变体ID) :{difficulty（可自定义，不一定非得是easy,normal,hard） : SongDifficulty}}
     */
    std::map<std::string, std::map<std::string, SongDifficulty>> difficulties;

    // 构造函数
    Song(const std::string& songId, const std::map<std::string, SongMetaData>& allMetadata)
    {
        this->id = songId;
        this->metadataMap = allMetadata;
        populateDifficulties();
    }

    // 填充难度
    void populateDifficulties() {
        for (const auto& [variation, metadata] : metadataMap) {
            for (const auto& diffId : metadata.playData.difficulties) {
                difficulties[variation][diffId] = SongDifficulty(diffId, variation, metadata);
            }
        }
    }

    // 根据角色获取可用变体
    std::vector<std::string> getVariationsByCharacter(const std::string& characterId) const {
        std::vector<std::string> result;
        for (const auto& [variation, metadata] : metadataMap) {
            if (metadata.playData.characters.player == characterId) {
                result.push_back(variation);
            }
        }
        return result;
    }

    // 列出可用难度
    std::vector<std::string> listDifficulties(const std::vector<std::string>& variations) const {
        std::set<std::string> result;
        for (const auto& vari : variations) {
            auto it = difficulties.find(vari);
            if (it != difficulties.end()) {
                for (const auto& [diff, _] : it->second) {
                    result.insert(diff);
                }
            }
        }
        return std::vector<std::string>(result.begin(), result.end());
    }

    // 获取特定难度数据
    std::optional<SongDifficulty> getDifficulty(const std::string& diffId, const std::string& variation) const {
        auto itVari = difficulties.find(variation);
        if (itVari != difficulties.end()) {
            auto itDiff = itVari->second.find(diffId);
            if (itDiff != itVari->second.end()) {
                return itDiff->second;
            }
        }
        return std::nullopt;
    }
};
