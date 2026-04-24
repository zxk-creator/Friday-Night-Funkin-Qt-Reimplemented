//
// Created by kkplay on 4/24/26.
//

#include "data/level/LevelData.h"

void LevelData_from_json_VS(const json& j, LevelData& data)
{
    if (j.contains("version")) data.version = j["version"].get<std::string>();
    data.name = j.at("name").get<std::string>();
    data.titleAsset = j.at("titleAsset").get<std::string>();
    if (j.contains("props"))
    {
        data.props.clear();
        for (auto& item : j["props"]) {
            LevelPropData prop;
            LevelPropData_from_json_VS(item, prop); // 逐个解析数组项
            data.props.push_back(prop);
        }
    }
    if (j.contains("visible")) data.visible = j["visible"].get<bool>();
    data.songs = j.at("songs").get<std::vector<std::string>>();
    if (j.contains("background")) data.background = j["background"].get<std::string>();
}

void LevelPropData_from_json_VS(const json& j, LevelPropData& data)
{
    data.assetPath = j.at("assetPath").get<std::string>();
    if (j.contains("scale")) data.scale = j["scale"].get<float>();
    if (j.contains("alpha")) data.alpha = j["alpha"].get<float>();
    if (j.contains("isPixel")) data.isPixel = j["isPixel"].get<bool>();
    if (j.contains("danceEvery")) data.danceEvery = j["danceEvery"].get<float>();
    if (j.contains("offsets")) data.offsets = j["offsets"].get<std::vector<float>>();
    if (j.contains("animations")) data.animations = j["animations"].get<std::vector<AnimationData>>();
    if (j.contains("startingAnimation")) data.startingAnimation = j["startingAnimation"].get<std::string>();
    if (j.contains("flipX")) data.flipX = j["flipX"].get<bool>();
}