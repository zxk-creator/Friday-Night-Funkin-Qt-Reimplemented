//
// Created by kkplay on 4/24/26.
//

#include "data/level/LevelData.h"
#include "Constants.h"
#include "data/Context.h"
#include "utils/VersionUtil.h"
#include "utils/exception/CustomException.h"
#include "utils/lang/LangStringPool.h"
#include "utils/message/MessageHandler.h"

using namespace std;

void LevelPropData::from_json(const json& j)
{
    // assetPath 是必填字段，如果缺失则记录错误并设置空值
    if (j.contains("assetPath") && j["assetPath"].is_string()) {
        assetPath = QString::fromStdString(j["assetPath"].get<std::string>());
    } else {
        MessageHandler::logError(false,QString("LevelPropData 缺少必填字段 assetPath，已设置为空字符串"), "LevelPropData");
        assetPath = "";
    }

    // scale（可选，默认 1.0）
    if (j.contains("scale") && j["scale"].is_number()) {
        scale = j["scale"].get<float>();
    } else if (j.contains("scale")) {
        MessageHandler::logWarning(false,QString("LevelPropData 的 scale 字段类型错误，应为数字，使用默认值 1.0"), "LevelPropData");
        scale = 1.0f;
    } else {
        scale = 1.0f;
    }

    // alpha（可选，默认 1.0）
    if (j.contains("alpha") && j["alpha"].is_number()) {
        alpha = j["alpha"].get<float>();
    } else if (j.contains("alpha")) {
        MessageHandler::logWarning(false,QString("LevelPropData 的 alpha 字段类型错误，应为数字，使用默认值 1.0"), "LevelPropData");
        alpha = 1.0f;
    } else {
        alpha = 1.0f;
    }

    // isPixel（可选，默认 false）
    if (j.contains("isPixel") && j["isPixel"].is_boolean()) {
        isPixel = j["isPixel"].get<bool>();
    } else if (j.contains("isPixel")) {
        MessageHandler::logWarning(false,QString("LevelPropData 的 isPixel 字段类型错误，应为布尔值，使用默认值 false"), "LevelPropData");
        isPixel = false;
    } else {
        isPixel = false;
    }

    // danceEvery（可选，默认 1.0）
    if (j.contains("danceEvery") && j["danceEvery"].is_number()) {
        danceEvery = j["danceEvery"].get<float>();
    } else if (j.contains("danceEvery")) {
        MessageHandler::logWarning(false,QString("LevelPropData 的 danceEvery 字段类型错误，应为数字，使用默认值 1.0"), "LevelPropData");
        danceEvery = 1.0f;
    } else {
        danceEvery = 1.0f;
    }

    // offsets（可选，默认 [0, 0]）
    if (j.contains("offsets") && j["offsets"].is_array()) {
        try {
            offsets = j["offsets"].get<std::vector<float>>();
        } catch (const std::exception& e) {
            MessageHandler::logWarning(false,QString("LevelPropData 的 offsets 字段解析失败：%1，使用默认值 [0, 0]").arg(e.what()), "LevelPropData");
            offsets = {0.0f, 0.0f};
        }
    } else if (j.contains("offsets")) {
        MessageHandler::logWarning(false,QString("LevelPropData 的 offsets 字段类型错误，应为数组，使用默认值 [0, 0]"), "LevelPropData");
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
            MessageHandler::logWarning(false,QString("LevelPropData 的 animations 字段解析失败：%1，使用空数组").arg(e.what()), "LevelPropData");
            animations.clear();
        }
    } else if (j.contains("animations")) {
        MessageHandler::logWarning(false,QString("LevelPropData 的 animations 字段类型错误，应为数组，使用空数组"), "LevelPropData");
        animations.clear();
    } else {
        animations.clear();
    }

    // startingAnimation（可选，默认空字符串）
    if (j.contains("startingAnimation") && j["startingAnimation"].is_string()) {
        startingAnimation = QString::fromStdString(j["startingAnimation"].get<std::string>());
    } else if (j.contains("startingAnimation")) {
        MessageHandler::logWarning(false,QString("LevelPropData 的 startingAnimation 字段类型错误，应为字符串，使用空字符串"), "LevelPropData");
        startingAnimation = "";
    } else {
        startingAnimation = "";
    }

    // 是否翻转
    if (j.contains("flipX") && j["flipX"].is_boolean()) {
        flipX = j["flipX"].get<bool>();
    } else if (j.contains("flipX")) {
        MessageHandler::logWarning(false,QString("LevelPropData 的 flipX 字段类型错误，应为布尔值，使用默认值 false"), "LevelPropData");
        flipX = false;
    } else {
        flipX = false;
    }
}

bool LevelData::from_json(const json& j)
{
    // 解析 version
    if (j.contains("version") && j["version"].is_string()) {
        version = QString::fromStdString(j["version"].get<std::string>());
    } else {
        version = LevelDataRelative::LEVEL_DATA_VERSION;
    }

    // 必填字段name
    if (!j.contains("name") || !j["name"].is_string()) {
        MessageHandler::logError(false,QString("缺少字段 'name'，将使用默认值。"), "LevelData");
        name = Context::lang->unknownWeekName();
    }
    else name = QString::fromStdString(j["name"].get<std::string>());

    // week1，week2那个艺术字图片
    if (!j.contains("titleAsset") || !j["titleAsset"].is_string()) {
        MessageHandler::logError(false,QString("缺少字段'titleAsset',将使用默认值。"), "LevelData");
        titleAsset = PathVS::image("/storymenu/unknownWeek").value();
    }
    // 多一个/是为了防御型编程，所以一定要使用QDir::cleanPath进行去除路径！
    else titleAsset = QDir::separator() + QString::fromStdString(j["titleAsset"].get<std::string>());

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
        MessageHandler::logError(false,QString("缺少字段 'songs'"), "LevelData");
        return false;
    }

    for (const auto& song : j["songs"]) {
        if (song.is_string()) {
            songs.push_back(QString::fromStdString(song.get<std::string>()));
        } else {
            MessageHandler::logWarning(false,QString("songs 数组元素类型错误，应为字符串"), "LevelData");
        }
    }

    // 背景颜色
    if (j.contains("background") && j["background"].is_string()) {
        background = QString::fromStdString(j["background"].get<std::string>());
    } else {
        background = "#F9CF51";
    }

    return true;
}

std::unique_ptr<LevelData> LevelDataParser::parseLevelData_VS(const json& j, const QString& filename)
{
    auto data = std::make_unique<LevelData>();

    if (!data->from_json(j)) {

        MessageHandler::logError(false,QString("解析关卡数据失败，必填字段缺失: %1").arg(filename), "LevelDataParser");
        return nullptr;
    }

    // 版本验证
    if (!VersionUtil::validateVersionStr(data->version, LevelDataRelative::LEVEL_DATA_VERSION_RULE)) {
        Exception::logVersionInvalid(data->version, LevelDataRelative::LEVEL_DATA_VERSION_RULE, filename);
        return nullptr;
    }

    MessageHandler::logInfo(QString("成功加载了关卡: %1").arg(data->name), "LevelDataParser");
    return data;
}

unique_ptr<LevelData> LevelDataParser::parseLevelData_PE(const json& j, const QString& filename)
{
    auto levelData_PE = make_unique<LevelData>();

    // 实际上PE没有版本号，我是随便设置的。
    levelData_PE->version = LevelDataRelative::LEVEL_DATA_VERSION;

    // 1. 基本信息
    if (j.contains("storyName")) {
        levelData_PE->name = QString::fromStdString(j["storyName"].get<std::string>());
    }
    else if (j.contains("weekName")) {
        // 如果 storyName 没有，则用 weekName
        if (levelData_PE->name.isEmpty()) {
            levelData_PE->name = QString::fromStdString(j["weekName"].get<std::string>());
        }
    }
    else levelData_PE->name = Context::lang->unknownWeekName();

    // 背景图片，注意拼出来的是相对路径，需要运行时在registry内进行拼接（注意一定要用cleanPath去除掉多余的分隔符号！）
    if (j.contains("weekBackground")) {
        QString bgName = "menu_" + QString::fromStdString(j["weekBackground"].get<std::string>());
        levelData_PE->background = "/images/menubackgrounds/" + bgName;
    }

    // 不做额外配置了，到时候解析的时候需要从对应的modMetadata内获取到引擎种类，然后按照对应的配置进行解析。

    // hideStoryMode为true时隐藏！
    if (j.contains("hideStoryMode") && j["hideStoryMode"].is_boolean()) {
        levelData_PE->visible = !j["hideStoryMode"].get<bool>();
    } else {
        levelData_PE->visible = true;
    }

    // 4. 歌曲列表：提取每首歌的第0个元素（歌名），其他字段我将忽略。
    if (j.contains("songs") && j["songs"].is_array()) {
        levelData_PE->songs.clear();
        for (const auto& songEntry : j["songs"]) {
            if (songEntry.is_array() && songEntry.size() > 0) {
                QString songName = QString::fromStdString(songEntry[0].get<std::string>());
                levelData_PE->songs.push_back(songName);
            }
        }
    }

    // 5. 标题图片：从modPath/images/storymenu获取
    // TODO: 需要支持level背景图片！

    // 这个就是storymenu黄色跳舞的那几个角色！
    if (j.contains("weekCharacters") && j["weekCharacters"].is_array()) {
        levelData_PE->props.clear();
        const auto& chars = j["weekCharacters"];

        // 默认位置偏移：左边(dad)、中间(bf)、右边(gf)
        std::vector<float> defaultPositions = {-200, 0, 200};  // X 方向偏移

        for (size_t i = 0; i < chars.size() && i < 3; i++) {
            LevelPropData prop;

            QString charName = QString::fromStdString(chars[i].get<std::string>());

            // 直接拼好路径（TODO: 注意，这里拼接的是json路径！若要识别图片，需要进一步解析JSON后读取里面的内容！这里先存json路径。）
            QString relativePath = "/images/menucharacters/" + charName;

            // 设置偏移量（X 坐标）
            if (i < defaultPositions.size()) {
                prop.offsets = {defaultPositions[i], 0};
            }

            // TODO: 这些都只能通过解析上述relative所指向的json后读取内容才能获得！后面再说
            prop.scale = 1.0f;
            prop.flipX = false;
            prop.isPixel = false;
            prop.danceEvery = 1.0f;
            prop.alpha = 1.0f;

            levelData_PE->props.push_back(prop);
        }
    }

    return levelData_PE;
}