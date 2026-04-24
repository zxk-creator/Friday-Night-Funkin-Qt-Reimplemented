//
// Created by kkplay on 4/22/26.
//

#pragma once

#include "nlohmann/json.hpp"

using json = nlohmann::json;

struct TextureAtlasData {
    bool swfMode = false;
    bool cacheOnLoad = false;
    int filterQuality = 1;      // 0=HIGH,1=MEDIUM,2=LOW,3=RUDY
    bool applyStageMatrix = false;
    bool useRenderTexture = false;
};

inline void to_json(json& j, const TextureAtlasData& d) {
    j = json{
            {"swfMode", d.swfMode},
            {"cacheOnLoad", d.cacheOnLoad},
            {"filterQuality", d.filterQuality},
            {"applyStageMatrix", d.applyStageMatrix},
            {"useRenderTexture", d.useRenderTexture}
    };
}

inline void from_json(const json& j, TextureAtlasData& d) {
    if (j.contains("swfMode")) d.swfMode = j["swfMode"].get<bool>();
    if (j.contains("cacheOnLoad")) d.cacheOnLoad = j["cacheOnLoad"].get<bool>();
    if (j.contains("filterQuality")) d.filterQuality = j["filterQuality"].get<int>();
    if (j.contains("applyStageMatrix")) d.applyStageMatrix = j["applyStageMatrix"].get<bool>();
    if (j.contains("useRenderTexture")) d.useRenderTexture = j["useRenderTexture"].get<bool>();
}