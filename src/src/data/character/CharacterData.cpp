//
// Created by kkplay on 5/2/26.
//

#include "data/character/CharacterData.h"

#include "utils/message/MessageHandler.h"

void HealthIconData::from_json(const json& j) {
    if (j.contains("id")) id = QString::fromStdString(j["id"].get<std::string>());
    if (j.contains("scale")) scale = j["scale"].get<float>();
    if (j.contains("flipX")) flipX = j["flipX"].get<bool>();
    if (j.contains("isPixel")) isPixel = j["isPixel"].get<bool>();
    if (j.contains("offsets") && j["offsets"].is_array()) {
        QVector<float> offs;
        for (const auto& val : j["offsets"]) {
            offs.push_back(val.get<float>());
        }
        offsets = offs;
    }
}

void DeathData::from_json(const json& j) {
    if (j.contains("cameraOffsets") && j["cameraOffsets"].is_array()) {
        QVector<float> offs;
        for (const auto& val : j["cameraOffsets"]) {
            offs.push_back(val.get<float>());
        }
        cameraOffsets = offs;
    }
    if (j.contains("cameraZoom")) cameraZoom = j["cameraZoom"].get<float>();
    if (j.contains("preTransitionDelay")) preTransitionDelay = j["preTransitionDelay"].get<float>();
}

CharacterRenderType parseRenderType(const QString& str) {
    if (str == "sparrow") return CharacterRenderType::Sparrow;
    if (str == "packer") return CharacterRenderType::Packer;
    if (str == "multisparrow") return CharacterRenderType::MultiSparrow;
    if (str == "animateatlas") return CharacterRenderType::AnimateAtlas;
    if (str == "multianimateatlas") return CharacterRenderType::MultiAnimateAtlas;
    if (str == "custom") return CharacterRenderType::Custom;
    return CharacterRenderType::Sparrow;
}

void CharacterData::from_json(const json& j) {
    if (j.contains("version")) version = QString::fromStdString(j["version"].get<std::string>());
    if (j.contains("name")) name = QString::fromStdString(j["name"].get<std::string>());
    if (j.contains("renderType")) renderType = parseRenderType(QString::fromStdString(j["renderType"].get<std::string>()));
    if (j.contains("assetPath")) assetPath = QString::fromStdString(j["assetPath"].get<std::string>());
    if (j.contains("scale")) scale = j["scale"].get<float>();
    if (j.contains("healthIcon")) {
        healthIcon = HealthIconData();
        healthIcon.from_json(j["healthIcon"]);
    }
    if (j.contains("death")) {
        death = DeathData();
        death.from_json(j["death"]);
    }
    if (j.contains("offsets") && j["offsets"].is_array()) {
        QVector<float> offs;
        for (const auto& val : j["offsets"]) {
            offs.push_back(val.get<float>());
        }
        offsets = offs;
    }
    // 没事，如果版本差异导致不包含也没问题，使用默认值
    if (j.contains("cameraOffsets") && j["cameraOffsets"].is_array()) {
        QVector<float> offs;
        for (const auto& val : j["cameraOffsets"]) {
            offs.push_back(val.get<float>());
        }
        cameraOffsets = offs;
    }
    if (j.contains("isPixel")) isPixel = j["isPixel"].get<bool>();
    if (j.contains("danceEvery")) danceEvery = j["danceEvery"].get<float>();
    if (j.contains("singTime")) singTime = j["singTime"].get<float>();
    if (j.contains("startingAnimation")) startingAnimation = QString::fromStdString(j["startingAnimation"].get<std::string>());
    if (j.contains("flipX")) flipX = j["flipX"].get<bool>();
    if (j.contains("applyStageMatrix")) applyStageMatrix = j["applyStageMatrix"].get<bool>();
    if (j.contains("atlasSettings")) {
        atlasSettings = TextureAtlasData();
        atlasSettings.from_json(j["atlasSettings"]);
    }

    // 解析动画数组
    if (j.contains("animations") && j["animations"].is_array()) {
        animations.clear();
        for (const auto& animJson : j["animations"]) {
            AnimationData anim;
            anim.from_json(animJson);
            animations.push_back(anim);
        }
    }
}

std::unique_ptr<CharacterData> parseCharacterData(const json& j, const QString& filepath)
{
    try {
        auto data = std::make_unique<CharacterData>();
        data->from_json(j);

        // 验证必填字段
        if (data->assetPath.isEmpty()) {
            MessageHandler::logWarning(
                filepath + "的assetPath字段为空！已跳过",
                "CharacterParser"
            );
            return nullptr;
        }

        if (data->animations.isEmpty()) {
            MessageHandler::logWarning(
                filepath + "的animations字段为空！已跳过",
                "CharacterParser"
            );
            return nullptr;
        }

        return data;
    } catch (const std::exception& e) {
        MessageHandler::logError(
            "解析模组" + filepath + "失败，原因: \n" + e.what() + "\n",
            "CharacterParser"
        );
        return nullptr;
    }
}