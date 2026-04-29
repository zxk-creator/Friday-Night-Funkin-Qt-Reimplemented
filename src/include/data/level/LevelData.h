//
// Created by kkplay on 4/22/26.
//

#pragma once
#include "data/animation/AnimationData.h"
#include "modding/ModEngineType.h"
#include <QDebug>

struct LevelPropData {
    std::string assetPath;
    float scale = 1.0f;
    float alpha = 1.0f;
    bool isPixel = false;
    float danceEvery = 1.0f;
    std::vector<float> offsets = {0, 0};
    std::vector<AnimationData> animations;
    std::string startingAnimation = "";
    bool flipX = false;
};

/**
inline void to_json(json& j, const LevelPropData& data) {
    j["assetPath"] = data.assetPath;
    if (data.scale != 1.0f) j["scale"] = data.scale;
    if (data.alpha != 1.0f) j["alpha"] = data.alpha;
    if (data.isPixel) j["isPixel"] = data.isPixel;
    if (data.danceEvery != 1.0f) j["danceEvery"] = data.danceEvery;
    if (data.offsets != std::vector<float>{0, 0}) j["offsets"] = data.offsets;
    if (!data.animations.empty()) j["animations"] = data.animations;
    if (!data.startingAnimation.empty()) j["startingAnimation"] = data.startingAnimation;
    if (data.flipX) j["flipX"] = data.flipX;
}
* tojson，由于格式比较复杂的原因，我们暂时不提供
*/


void LevelPropData_from_json_VS(const json& j, LevelPropData& data);
inline void LevelPropData_from_json_PE(const json& j, LevelPropData& data)
{
    // TODO:暂时还没适配PE
}

/**
 * leveldata是用来展示在storymenu中角色配置信息
 */
struct LevelData {
    std::string version = "1.0.1";
    std::string name;
    std::string titleAsset;
    std::vector<LevelPropData> props;
    // 也许这是隐藏曲？
    bool visible = true;
    // 关键字段：需要用这里面的内容在SongRegistry进行字符串查找从而获得指定Song对象进行播放！
    std::vector<std::string> songs = {"bopeebo"};
    std::string background = "#F9CF51";
};

/**
inline void to_json(json& j, const LevelData& data) {
    j["version"] = data.version;
    j["name"] = data.name;
    j["titleAsset"] = data.titleAsset;
    if (!data.props.empty()) j["props"] = data.props;
    if (!data.visible) j["visible"] = data.visible;
    j["songs"] = data.songs;
    if (data.background != "#F9CF51") j["background"] = data.background;
}
* tojson，由于格式比较复杂的原因，我们暂时不提供
*/

void LevelData_from_json_VS(const json& j, LevelData& data);
inline void LevelData_from_json_PE(const json& j, LevelData& data)
{
    // TODO:暂时还没适配PE
}
