//
// Created by kkplay on 4/22/26.
//

#pragma once
#include <optional>
#include <string>
#include <vector>

#include "TextureAtlas.h"

using json = nlohmann::json;

/**
 * 存的是动画的元数据配置（名称，前缀，偏移，帧率，是否循环等等，而不是每一帧的动画数据）
 * 人话：告诉动画系统如何从文件中取得数据，而不存储每一帧的数据
 */
struct UnnamedAnimationData : ISerializable {
    std::optional<QString> prefix;
    std::optional<QString> assetPath;
    std::vector<float> offsets = {0, 0};
    bool looped = false;
    bool flipX = false;
    bool flipY = false;
    int frameRate = 24;
    std::vector<int> frameIndices;
    QString animType = "framelabel";
    QString renderType;
    std::optional<TextureAtlasData> atlasSettings;

    void from_json(const json& j) {
        if (j.contains("prefix")) prefix = QString::fromStdString(j["prefix"].get<std::string>());
        if (j.contains("assetPath"))
        {
            // 由于我们的文件系统与Lime不同，因此需要改一下。
            assetPath = QString::fromStdString(j["assetPath"].get<std::string>());
            
        }
        if (j.contains("offsets")) offsets = j["offsets"].get<std::vector<float>>();
        if (j.contains("looped")) looped = j["looped"].get<bool>();
        if (j.contains("flipX")) flipX = j["flipX"].get<bool>();
        if (j.contains("flipY")) flipY = j["flipY"].get<bool>();
        if (j.contains("frameRate")) frameRate = j["frameRate"].get<int>();
        if (j.contains("frameIndices")) frameIndices = j["frameIndices"].get<std::vector<int>>();
        if (j.contains("animType")) animType = QString::fromStdString(j["animType"].get<std::string>());
        if (j.contains("renderType")) renderType = QString::fromStdString(j["renderType"].get<std::string>());
        if (j.contains("atlasSettings")) {
            TextureAtlasData t;
            t.from_json(j["atlasSettings"]);
            atlasSettings = t;
        }
    }

    QString toString() const override
    {
        QString res;
        res += "动画类型: " + animType + "\n";

        if (prefix.has_value()) {
            res += "前缀: " + prefix.value() + "\n";
        }
        if (assetPath.has_value()) {
            res += "资源路径: " + assetPath.value() + "\n";
        }

        res += "偏移: [" + QString::number(offsets[0]) + ", " + QString::number(offsets[1]) + "]\n";
        res += "循环: " + QString(looped ? "是" : "否") + "\n";
        res += "水平翻转: " + QString(flipX ? "是" : "否") + "\n";
        res += "垂直翻转: " + QString(flipY ? "是" : "否") + "\n";
        res += "帧率: " + QString::number(frameRate) + "\n";

        if (!frameIndices.empty()) {
            res += "帧索引: [";
            for (size_t i = 0; i < frameIndices.size(); ++i) {
                res += QString::number(frameIndices[i]);
                if (i < frameIndices.size() - 1) res += ", ";
            }
            res += "]\n";
        }

        res += "渲染类型: " + renderType + "\n";

        if (atlasSettings.has_value()) {
            res += "--- 图集设置 ---\n";
            res += atlasSettings.value().toString();
        }

        return res;
    }

    QString oneToString(const QString& id) const override
    {
        return toString();
    }
};

struct AnimationData : UnnamedAnimationData{
    QString name;

    void from_json(const json& j) {
        // 先调用父类的 from_json
        UnnamedAnimationData::from_json(j);
        // 再解析自己的成员
        if (j.contains("name")) name = QString::fromStdString(j["name"].get<std::string>());
    }

    QString toString() const override
    {
        QString res = "动画名称: " + name + "\n";
        res += UnnamedAnimationData::toString();
        return res;
    }

    QString oneToString(const QString& id) const override
    {
        return toString();
    }
};