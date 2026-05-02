//
// Created by kkplay on 4/22/26.
//

#pragma once
#include "data/animation/AnimationData.h"


#include "play/ISerializable.h"

/**
 *  艹了，C++没有反射，一切都得自己从零开始手写！！！！！！！
 *  艹
 *  2026,5,1
 */

/**
 * StoryMenu跳舞的那个角色
 */
struct LevelPropData {
    QString assetPath;
    float scale = 1.0f;
    float alpha = 1.0f;
    bool isPixel = false;
    float danceEvery = 1.0f;
    std::vector<float> offsets = {0, 0};
    std::vector<AnimationData> animations;
    QString startingAnimation = "";
    bool flipX = false;

    // nlohmann 自动转换
    void from_json(const json& j);
};

/**
 * 用来展示在storymenu中周目配置信息
 */
struct LevelData : ISerializable {
    QString version = "1.0.1";
    QString name;
    QString titleAsset;
    std::vector<LevelPropData> props;
    // 也许这是隐藏曲？
    bool visible = true;
    // 关键字段：需要用这里面的内容在SongRegistry进行字符串查找从而获得指定Song对象进行播放！
    std::vector<QString> songs = {"bopeebo"};
    QString background = "#F9CF51";

    QString toString() const override
    {
        QString division = "========== 周目信息 ==========\n";
        QString res = division + "版本: " + version + "\n名字: " + name + "\n是否隐藏: " + (visible ? "是" : "否") + "\n拥有的歌曲: ";
        for (auto& song : songs)
        {
            res += song + " ";
        }

        return res + "\n" + division + "\n\n";
    }

    QString oneToString(const QString& id) const override
    {
        return toString();
    }

    bool from_json(const json& j);
};

/**
 * LevelDataParser - 解析关卡数据
 * 解析逻辑在 .cpp 文件中
 */
class LevelDataParser {
public:
    static std::optional<LevelData> parseLevelData_VS(const json& j, const QString& filename);

    //（暂未适配）
    static void parseLevelData_PE(const json& j, LevelData& data);
};