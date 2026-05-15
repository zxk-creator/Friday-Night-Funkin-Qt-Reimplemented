//
// Created by kkplay on 4/22/26.
//

#pragma once

#include "nlohmann/json.hpp"
#include "data/animation/AnimationData.h"

using json = nlohmann::json;

// 什么意思？这代表一个数组，单独的一个float代表宽度和高度缩放1.5倍，数组则是第一个宽度缩放多少倍，第二个高度缩放多少倍。
struct StageDataCharacter : ISerializable {
    int zIndex = 0;
    QVector<float> position = {0, 0};
    float scale = 1.0f;
    QVector<float> cameraOffsets;
    QVector<float> scroll = {1, 1};
    float alpha = 1.0f;
    float angle = 0.0f;

    void from_json(const json& j);
    QString toString() const override;
    QString oneToString(const QString& id) const override
    {
        return toString();
    }
};

struct StagePropData : ISerializable {
    std::optional<QString> name;
    QString assetPath;
    QVector<float> position;
    int zIndex = 0;
    bool isPixel = false;
    bool flipX = false;
    bool flipY = false;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float alpha = 1.0f;
    float danceEvery = 0.0f;
    QVector<float> scroll = {1, 1};
    QVector<AnimationData> animations;
    std::optional<QString> startingAnimation;
    QString animType = "sparrow";
    float angle = 0.0f;
    QString blend = "";
    QString color = "#FFFFFF";
    std::optional<TextureAtlasData> atlasSettings;

    void from_json(const json& j);

    QString toString() const override;
    QString oneToString(const QString& id) const override
    {
        return toString();
    }
};

struct StageDataCharacters : ISerializable{
    StageDataCharacter bf;
    StageDataCharacter dad;
    StageDataCharacter gf;

    void from_json(const json& j);

    QString toString() const override;
    QString oneToString(const QString& id) const override
    {
        return toString();
    }
};


struct StageData : ISerializable{
    QString version;
    QString name = "Unknown";
    QVector<StagePropData> props;
    StageDataCharacters characters;
    std::optional<float> cameraZoom = 1.0f;
    std::optional<QString> directory = "shared";

    // 为了兼容PE而新增的字段
    float cameraSpeed = 1.0f;

    void from_json(const json& j);

    QString toString() const override;
    QString oneToString(const QString& id) const override
    {
        return toString();
    }
};

/**
 * 目前没看到版本迁移相关逻辑，因此我也不写了。最新版：1.0.2和1.0.0兼容。
 */
namespace StageDataParser
{
    std::unique_ptr<StageData> parseStageData_VS(const json& j, const QString& filepath = "");
    std::unique_ptr<StageData> parseStageData_PE(const json& j, const QString& filename = "");
}