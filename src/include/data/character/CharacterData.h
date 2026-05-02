//
// Created by kkplay on 5/2/26.
//

#pragma once

#include <QVector>

#include "nlohmann/json.hpp"
#include "data/animation/AnimationData.h"
#include "play/ISerializable.h"

using json = nlohmann::json;

/**
 * shit
 */

// 决定使用哪种图集系统来加载和播放角色的动画。
enum class CharacterRenderType {
    Sparrow,
    Packer,
    MultiSparrow,
    AnimateAtlas,
    MultiAnimateAtlas,
    Custom
};

struct HealthIconData : ISerializable
{
    QString id = "未提供";
    float scale = 1.0f;
    bool flipX = false;
    bool isPixel = false;
    QVector<float> offsets = QVector<float>{0, 25};

    // 想了想还是直接写到这里面吧，不写到外面了
    void from_json(const json& j);

    QString toString() const override
    {
        QString res;
        res += "--- 健康图标 ---\n";
        res += "ID: " + id + "\n";
        res += "缩放: " + QString::number(scale) + "\n";
        res += "水平翻转: " + QString(flipX ? "是" : "否") + "\n";
        res += "像素风格: " + QString(isPixel ? "是" : "否") + "\n";
        res += "偏移: [";
        for (int i = 0; i < offsets.size(); ++i) {
            res += QString::number(offsets[i]);
            if (i < offsets.size() - 1) res += ", ";
        }
        res += "]\n";
        return res;
    }

    QString oneToString(const QString& id) const override
    {
        return toString();
    }
};

struct DeathData : ISerializable {
    /**
    *  1.0.1加了 `death.cameraOffsets`这个字段。
    */
    QVector<float> cameraOffsets;
    float cameraZoom = 1.0f;
    float preTransitionDelay = 0.0f;

    void from_json(const json& j);

    QString toString() const override
    {
        QString res;
        res += "--- 死亡数据 ---\n";
        res += "摄像机偏移: [";
        for (int i = 0; i < cameraOffsets.size(); ++i) {
            res += QString::number(cameraOffsets[i]);
            if (i < cameraOffsets.size() - 1) res += ", ";
        }
        res += "]\n";
        res += "摄像机缩放: " + QString::number(cameraZoom) + "\n";
        res += "过渡延迟: " + QString::number(preTransitionDelay) + "\n";
        return res;
    }

    QString oneToString(const QString& id) const override
    {
        return toString();
    }
};

struct CharacterData : ISerializable {
    // 原版关于版本的差异较小而且没提供迁移逻辑之类的，所以我也不提供了，统一使用最新版
    QString version = "1.0.1";
    QString name = "未命名角色";
    CharacterRenderType renderType = CharacterRenderType::Sparrow;
    QString assetPath;
    float scale = 1.0f;
    HealthIconData healthIcon;
    DeathData death;
    QVector<float> offsets = QVector<float>{0, 0};
    QVector<float> cameraOffsets = QVector<float>{0, 0};
    bool isPixel = false;
    float danceEvery = 1.0f;
    float singTime = 8.0f;
    QVector<AnimationData> animations;
    QString startingAnimation = "idle";
    bool flipX = false;
    // 对于从Ae导出的符号动画，是否使用导出时的位置和变换信息。
    bool applyStageMatrix = false;
    // 为AnimateAtlas类型的角色提供额外的渲染配置。
    TextureAtlasData atlasSettings;

    void from_json(const json& j);

    QString toString() const override
    {
        QString division = "========== 角色数据 ==========\n";
        QString res = division;

        // 基本信息
        res += "版本: " + version + "\n";
        res += "名称: " + name + "\n";
        res += "渲染类型: " + renderTypeToString(renderType) + "\n";
        res += "资源路径: " + assetPath + "\n";
        res += "缩放: " + QString::number(scale) + "\n";
        res += "像素风格: " + QString(isPixel ? "是" : "否") + "\n";
        res += "水平翻转: " + QString(flipX ? "是" : "否") + "\n";
        res += "应用舞台矩阵: " + QString(applyStageMatrix ? "是" : "否") + "\n";

        // 偏移
        res += "偏移: [";
        for (int i = 0; i < offsets.size(); ++i) {
            res += QString::number(offsets[i]);
            if (i < offsets.size() - 1) res += ", ";
        }
        res += "]\n";

        res += "摄像机偏移: [";
        for (int i = 0; i < cameraOffsets.size(); ++i) {
            res += QString::number(cameraOffsets[i]);
            if (i < cameraOffsets.size() - 1) res += ", ";
        }
        res += "]\n";

        // 动画参数
        res += "舞蹈间隔: " + QString::number(danceEvery) + "\n";
        res += "唱歌时间: " + QString::number(singTime) + "\n";
        res += "起始动画: " + startingAnimation + "\n";

        // 健康图标
        res += "\n" + healthIcon.toString();

        // 死亡数据
        res += "\n" + death.toString();

        // 图集设置
        res += "\n--- 图集设置 ---\n";
        res += atlasSettings.toString();

        // 动画列表
        if (!animations.isEmpty()) {
            res += "\n--- 动画列表 (" + QString::number(animations.size()) + "个) ---\n";
            for (const auto& anim : animations) {
                res += anim.toString() + "\n";
            }
        }

        return res + "\n" + division;
    }

    QString oneToString(const QString& id) const override
    {
        return toString();
    }

private:
    QString renderTypeToString(CharacterRenderType type) const
    {
        switch (type) {
        case CharacterRenderType::Sparrow: return "Sparrow";
        case CharacterRenderType::Packer: return "Packer";
        case CharacterRenderType::MultiSparrow: return "MultiSparrow";
        case CharacterRenderType::AnimateAtlas: return "AnimateAtlas";
        case CharacterRenderType::MultiAnimateAtlas: return "MultiAnimateAtlas";
        case CharacterRenderType::Custom: return "Custom";
        default: return "Unknown";
        }
    }
};

// 可能为空，记得一定要检查再使用，否则崩溃！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
std::unique_ptr<CharacterData> parseCharacterData(const json& j, const QString& filepath);