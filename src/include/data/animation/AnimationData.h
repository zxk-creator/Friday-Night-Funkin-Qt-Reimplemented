//
// Created by kkplay on 4/22/26.
//

#pragma once
#include <optional>
#include <string>
#include <vector>

#include "data/stage/StageData.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

struct UnnamedAnimationData {
    std::optional<std::string> prefix;
    std::optional<std::string> assetPath;
    std::vector<float> offsets = {0, 0};
    bool looped = false;
    bool flipX = false;
    bool flipY = false;
    int frameRate = 24;
    std::vector<int> frameIndices;
    std::string animType = "framelabel";
    std::string renderType;
    std::optional<TextureAtlasData> atlasSettings;
};

struct AnimationData : public UnnamedAnimationData {
    std::string name;
};

// 无名结构体转换
inline void to_json(json& j, const UnnamedAnimationData& data) {
    if (data.prefix.has_value()) j["prefix"] = data.prefix.value();
    if (data.assetPath.has_value()) j["assetPath"] = data.assetPath.value();
    j["offsets"] = data.offsets;
    j["looped"] = data.looped;
    j["flipX"] = data.flipX;
    j["flipY"] = data.flipY;
    j["frameRate"] = data.frameRate;
    if (!data.frameIndices.empty()) j["frameIndices"] = data.frameIndices;
    j["animType"] = data.animType;
    if (!data.renderType.empty()) j["renderType"] = data.renderType;
    if (data.atlasSettings.has_value()) j["atlasSettings"] = data.atlasSettings.value();
}

// 没名结构体序列化json
inline void from_json(const json& j, UnnamedAnimationData& data) {
    if (j.contains("prefix")) data.prefix = j["prefix"].get<std::string>();
    if (j.contains("assetPath")) data.assetPath = j["assetPath"].get<std::string>();
    if (j.contains("offsets")) data.offsets = j["offsets"].get<std::vector<float>>();
    if (j.contains("looped")) data.looped = j["looped"].get<bool>();
    if (j.contains("flipX")) data.flipX = j["flipX"].get<bool>();
    if (j.contains("flipY")) data.flipY = j["flipY"].get<bool>();
    if (j.contains("frameRate")) data.frameRate = j["frameRate"].get<int>();
    if (j.contains("frameIndices")) data.frameIndices = j["frameIndices"].get<std::vector<int>>();
    if (j.contains("animType")) data.animType = j["animType"].get<std::string>();
    if (j.contains("renderType")) data.renderType = j["renderType"].get<std::string>();
    if (j.contains("atlasSettings")) data.atlasSettings = j["atlasSettings"].get<TextureAtlasData>();
}

// AnimationDataJSON转换
inline void to_json(json& j, const AnimationData& data) {
    to_json(j, static_cast<const UnnamedAnimationData&>(data));
    j["name"] = data.name;
}

inline void from_json(const json& j, AnimationData& data)
{
    from_json(j, static_cast<UnnamedAnimationData&>(data));
    data.name = j.at("name").get<std::string>();
}