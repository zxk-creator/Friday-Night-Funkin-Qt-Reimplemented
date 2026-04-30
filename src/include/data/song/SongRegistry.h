//
// Created by kkplay on 4/25/26.
//

#pragma once
#include "data/BaseRegistry.h"
#include "play/song/Song.h"

// 专门管Song对象存放的
class SongRegistry : public BaseRegistry
{
    // C++ variable name is really long and long and I can't stand with it anymore.
    // {id: 歌曲对象}
    std::unordered_map<std::string, std::unique_ptr<Song>> entries;
    static SongRegistry* s_instance;

public:
    SongRegistry() = default;

    // 我决定了，以后就用这种模式作为“单例模式”，不统一初始化了
    static SongRegistry* instance() {
        if (!s_instance) s_instance = new SongRegistry();
        return s_instance;
    }

    // 注册 Song
    void registerSong(const std::string& songId, const std::map<std::string, SongMetaData>& allMetadata) {
        entries.emplace(songId, Song(songId, allMetadata));
    }

    // 获取 Song
    std::optional<Song> fetchEntry(const std::string& songId) const {
        auto it = entries.find(songId);
        if (it != entries.end()) return it->second;
        return std::nullopt;
    }

    bool hasEntry(const std::string& songId) const {
        return entries.find(songId) != entries.end();
    }

    std::vector<std::string> listEntryIds() const {
        std::vector<std::string> ids;
        for (const auto& [id, _] : entries) ids.push_back(id);
        return ids;
    }
};
