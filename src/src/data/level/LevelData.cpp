//
// Created by kkplay on 4/24/26.
//

#include "data/level/LevelData.h"
#include "Constants.h"
#include "utils/VersionUtil.h"
#include "utils/exception/CustomException.h"
#include "utils/message/MessageHandler.h"

void LevelPropData::from_json(const json& j)
{
    // assetPath 是必填字段，如果缺失则记录错误并设置空值
    if (j.contains("assetPath") && j["assetPath"].is_string()) {
        assetPath = QString::fromStdString(j["assetPath"].get<std::string>());
    } else {
        MessageHandler::logError(QString("LevelPropData 缺少必填字段 assetPath，已设置为空字符串"), "LevelPropData");
        assetPath = "";
    }

    // scale（可选，默认 1.0）
    if (j.contains("scale") && j["scale"].is_number()) {
        scale = j["scale"].get<float>();
    } else if (j.contains("scale")) {
        MessageHandler::logWarning(QString("LevelPropData 的 scale 字段类型错误，应为数字，使用默认值 1.0"), "LevelPropData");
        scale = 1.0f;
    } else {
        scale = 1.0f;
    }

    // alpha（可选，默认 1.0）
    if (j.contains("alpha") && j["alpha"].is_number()) {
        alpha = j["alpha"].get<float>();
    } else if (j.contains("alpha")) {
        MessageHandler::logWarning(QString("LevelPropData 的 alpha 字段类型错误，应为数字，使用默认值 1.0"), "LevelPropData");
        alpha = 1.0f;
    } else {
        alpha = 1.0f;
    }

    // isPixel（可选，默认 false）
    if (j.contains("isPixel") && j["isPixel"].is_boolean()) {
        isPixel = j["isPixel"].get<bool>();
    } else if (j.contains("isPixel")) {
        MessageHandler::logWarning(QString("LevelPropData 的 isPixel 字段类型错误，应为布尔值，使用默认值 false"), "LevelPropData");
        isPixel = false;
    } else {
        isPixel = false;
    }

    // danceEvery（可选，默认 1.0）
    if (j.contains("danceEvery") && j["danceEvery"].is_number()) {
        danceEvery = j["danceEvery"].get<float>();
    } else if (j.contains("danceEvery")) {
        MessageHandler::logWarning(QString("LevelPropData 的 danceEvery 字段类型错误，应为数字，使用默认值 1.0"), "LevelPropData");
        danceEvery = 1.0f;
    } else {
        danceEvery = 1.0f;
    }

    // offsets（可选，默认 [0, 0]）
    if (j.contains("offsets") && j["offsets"].is_array()) {
        try {
            offsets = j["offsets"].get<std::vector<float>>();
        } catch (const std::exception& e) {
            MessageHandler::logWarning(QString("LevelPropData 的 offsets 字段解析失败：%1，使用默认值 [0, 0]").arg(e.what()), "LevelPropData");
            offsets = {0.0f, 0.0f};
        }
    } else if (j.contains("offsets")) {
        MessageHandler::logWarning(QString("LevelPropData 的 offsets 字段类型错误，应为数组，使用默认值 [0, 0]"), "LevelPropData");
        offsets = {0.0f, 0.0f};
    } else {
        offsets = {0.0f, 0.0f};
    }

    // animations（可选，默认空数组）
    if (j.contains("animations") && j["animations"].is_array()) {
        try {
            animations.clear();
            for (const auto& animJson : j["animations"]) {
                AnimationData ad;
                ad.from_json(animJson);
                animations.push_back(ad);
            }
        } catch (const std::exception& e) {
            MessageHandler::logWarning(QString("LevelPropData 的 animations 字段解析失败：%1，使用空数组").arg(e.what()), "LevelPropData");
            animations.clear();
        }
    } else if (j.contains("animations")) {
        MessageHandler::logWarning(QString("LevelPropData 的 animations 字段类型错误，应为数组，使用空数组"), "LevelPropData");
        animations.clear();
    } else {
        animations.clear();
    }

    // startingAnimation（可选，默认空字符串）
    if (j.contains("startingAnimation") && j["startingAnimation"].is_string()) {
        startingAnimation = QString::fromStdString(j["startingAnimation"].get<std::string>());
    } else if (j.contains("startingAnimation")) {
        MessageHandler::logWarning(QString("LevelPropData 的 startingAnimation 字段类型错误，应为字符串，使用空字符串"), "LevelPropData");
        startingAnimation = "";
    } else {
        startingAnimation = "";
    }

    // 是否翻转
    if (j.contains("flipX") && j["flipX"].is_boolean()) {
        flipX = j["flipX"].get<bool>();
    } else if (j.contains("flipX")) {
        MessageHandler::logWarning(QString("LevelPropData 的 flipX 字段类型错误，应为布尔值，使用默认值 false"), "LevelPropData");
        flipX = false;
    } else {
        flipX = false;
    }
}

bool LevelData::from_json(const json& j)
{
    // 解析 version（可选，有默认值）
    if (j.contains("version") && j["version"].is_string()) {
        version = QString::fromStdString(j["version"].get<std::string>());
    } else {
        version = LevelDataRelative::LEVEL_DATA_VERSION;
    }

    // 必填字段name
    if (!j.contains("name") || !j["name"].is_string()) {
        MessageHandler::logError(QString("缺少字段 'name'"), "LevelData");
        return false;
    }
    name = QString::fromStdString(j["name"].get<std::string>());

    // 必填字段：titleAsset
    if (!j.contains("titleAsset") || !j["titleAsset"].is_string()) {
        MessageHandler::logError(QString("缺少字段'titleAsset'"), "LevelData");
        return false;
    }
    titleAsset = QString::fromStdString(j["titleAsset"].get<std::string>());

    // 可选字段：props
    if (j.contains("props") && j["props"].is_array()) {
        for (const auto& item : j["props"]) {
            LevelPropData prop;
            prop.from_json(item);
            props.push_back(prop);
        }
    }

    // 可选字段：visible（默认 true）
    if (j.contains("visible") && j["visible"].is_boolean()) {
        visible = j["visible"].get<bool>();
    } else {
        visible = true;
    }

    // 必填字段：songs
    if (!j.contains("songs") || !j["songs"].is_array()) {
        MessageHandler::logError(QString("缺少字段 'songs'"), "LevelData");
        return false;
    }

    for (const auto& song : j["songs"]) {
        if (song.is_string()) {
            songs.push_back(QString::fromStdString(song.get<std::string>()));
        } else {
            MessageHandler::logWarning(QString("songs 数组元素类型错误，应为字符串"), "LevelData");
        }
    }

    // 可选字段：background（默认 "#F9CF51"）
    if (j.contains("background") && j["background"].is_string()) {
        background = QString::fromStdString(j["background"].get<std::string>());
    } else {
        background = "#F9CF51";
    }

    return true;
}

std::optional<LevelData> LevelDataParser::parseLevelData_VS(const json& j, const QString& filename)
{
    LevelData data;

    if (!data.from_json(j)) {

        MessageHandler::logError(QString("解析关卡数据失败，必填字段缺失: %1").arg(filename), "LevelDataParser");
        return std::nullopt;
    }

    // 版本验证
    if (!VersionUtil::validateVersionStr(data.version, LevelDataRelative::LEVEL_DATA_VERSION)) {
        Exception::logVersionInvalid(data.version, LevelDataRelative::LEVEL_DATA_VERSION, filename);
        return std::nullopt;
    }

    MessageHandler::logInfo(QString("成功加载了关卡: %1").arg(data.name), "LevelDataParser");
    return data;
}

void LevelDataParser::parseLevelData_PE(const json& j, LevelData& data)
{
    // TODO:暂时还没适配PE
}