//
// Created by kkplay on 5/3/26.
//

#pragma once

#include <optional>
#include <memory>
#include "data/animation/AnimationData.h"
#include "nlohmann/json.hpp"
#include "utils/message/MessageHandler.h"

using json = nlohmann::json;

enum class NoteDirection {
    Left,
    Down,
    Up,
    Right
};

template<typename T>
struct NoteStyleAssetData : ISerializable {
    QString assetPath;
    float scale = 1.0f;
    QVector<float> offsets = {0, 0};
    bool isPixel = false;
    float alpha = 1.0f;
    bool animated = false;
    std::optional<T> data;

    void from_json(const json& j) {
        if (j.contains("assetPath")) assetPath = QString::fromStdString(j["assetPath"].get<std::string>());
        if (j.contains("scale")) scale = j["scale"].get<float>();
        if (j.contains("offsets") && j["offsets"].is_array()) {
            offsets.clear();
            for (const auto& val : j["offsets"]) {
                offsets.push_back(val.get<float>());
            }
        }
        if (j.contains("isPixel")) isPixel = j["isPixel"].get<bool>();
        if (j.contains("alpha")) alpha = j["alpha"].get<float>();
        if (j.contains("animated")) animated = j["animated"].get<bool>();
        if (j.contains("data") && !j["data"].is_null()) {
            data = T();
            data->from_json(j["data"]);
        }
    }

    QString toString() const override;
    QString oneToString(const QString& id) const override {
        return toString();
    }
};

struct NoteStyleData_Note : ISerializable {
    UnnamedAnimationData left;
    UnnamedAnimationData down;
    UnnamedAnimationData up;
    UnnamedAnimationData right;

    void from_json(const json& j);
    QString toString() const override;
    QString oneToString(const QString& id) const override {
        return toString();
    }
};

struct NoteStyleData_Countdown : ISerializable {
    QString audioPath;

    void from_json(const json& j);
    QString toString() const override;
    QString oneToString(const QString& id) const override {
        return toString();
    }
};

struct NoteStyleData_NoteStrumline : ISerializable {
    UnnamedAnimationData leftStatic;
    UnnamedAnimationData leftPress;
    UnnamedAnimationData leftConfirm;
    UnnamedAnimationData leftConfirmHold;
    UnnamedAnimationData downStatic;
    UnnamedAnimationData downPress;
    UnnamedAnimationData downConfirm;
    UnnamedAnimationData downConfirmHold;
    UnnamedAnimationData upStatic;
    UnnamedAnimationData upPress;
    UnnamedAnimationData upConfirm;
    UnnamedAnimationData upConfirmHold;
    UnnamedAnimationData rightStatic;
    UnnamedAnimationData rightPress;
    UnnamedAnimationData rightConfirm;
    UnnamedAnimationData rightConfirmHold;

    void from_json(const json& j);
    QString toString() const override;
    QString oneToString(const QString& id) const override {
        return toString();
    }
};

struct NoteStyleData_NoteSplash : ISerializable {
    std::optional<bool> enabled = true;
    std::optional<int> framerateDefault = 24;
    std::optional<int> framerateVariance = 2;
    std::optional<QString> blendMode = "normal";
    std::optional<QVector<UnnamedAnimationData>> leftSplashes;
    std::optional<QVector<UnnamedAnimationData>> downSplashes;
    std::optional<QVector<UnnamedAnimationData>> upSplashes;
    std::optional<QVector<UnnamedAnimationData>> rightSplashes;

    void from_json(const json& j);
    QString toString() const override;
    QString oneToString(const QString& id) const override {
        return toString();
    }
};

struct NoteStyleData_HoldNoteCoverDirectionData : ISerializable {
    std::optional<QString> assetPath;
    std::optional<UnnamedAnimationData> start;
    std::optional<UnnamedAnimationData> hold;
    std::optional<UnnamedAnimationData> end;

    void from_json(const json& j);
    QString toString() const override;
    QString oneToString(const QString& id) const override {
        return toString();
    }
};

struct NoteStyleData_HoldNoteCover : ISerializable {
    std::optional<bool> enabled = true;
    std::optional<NoteStyleData_HoldNoteCoverDirectionData> left;
    std::optional<NoteStyleData_HoldNoteCoverDirectionData> down;
    std::optional<NoteStyleData_HoldNoteCoverDirectionData> up;
    std::optional<NoteStyleData_HoldNoteCoverDirectionData> right;

    void from_json(const json& j);
    QString toString() const override;
    QString oneToString(const QString& id) const override {
        return toString();
    }
};

struct NoteStyleData_ComboNum : ISerializable {
    // 目前没有字段，预留
    void from_json(const json& j);
    QString toString() const override;
    QString oneToString(const QString& id) const override {
        return toString();
    }
};

struct NoteStyleData_Judgement : ISerializable {
    // 目前没有字段，预留
    void from_json(const json& j);
    QString toString() const override;
    QString oneToString(const QString& id) const override {
        return toString();
    }
};

struct NoteStyleData_HoldNote : ISerializable {
    // 这个haxe源码并没有定义，所以我也不定义了
    void from_json(const json& j) {}
    QString toString() const override {
        return "长按音符数据: 空(未定义)";
    }
    QString oneToString(const QString& id) const override {
        return toString();
    }
};

struct NoteStyleAssetsData : ISerializable {
    std::optional<NoteStyleAssetData<NoteStyleData_Note>> note;
    std::optional<NoteStyleAssetData<NoteStyleData_HoldNote>> holdNote;
    std::optional<NoteStyleAssetData<NoteStyleData_NoteStrumline>> noteStrumline;
    std::optional<NoteStyleAssetData<NoteStyleData_NoteSplash>> noteSplash;
    std::optional<NoteStyleAssetData<NoteStyleData_HoldNoteCover>> holdNoteCover;
    std::optional<NoteStyleAssetData<NoteStyleData_Countdown>> countdownThree;
    std::optional<NoteStyleAssetData<NoteStyleData_Countdown>> countdownTwo;
    std::optional<NoteStyleAssetData<NoteStyleData_Countdown>> countdownOne;
    std::optional<NoteStyleAssetData<NoteStyleData_Countdown>> countdownGo;
    std::optional<NoteStyleAssetData<NoteStyleData_Judgement>> judgementSick;
    std::optional<NoteStyleAssetData<NoteStyleData_Judgement>> judgementGood;
    std::optional<NoteStyleAssetData<NoteStyleData_Judgement>> judgementBad;
    std::optional<NoteStyleAssetData<NoteStyleData_Judgement>> judgementShit;
    std::optional<NoteStyleAssetData<NoteStyleData_ComboNum>> comboNumber0;
    std::optional<NoteStyleAssetData<NoteStyleData_ComboNum>> comboNumber1;
    std::optional<NoteStyleAssetData<NoteStyleData_ComboNum>> comboNumber2;
    std::optional<NoteStyleAssetData<NoteStyleData_ComboNum>> comboNumber3;
    std::optional<NoteStyleAssetData<NoteStyleData_ComboNum>> comboNumber4;
    std::optional<NoteStyleAssetData<NoteStyleData_ComboNum>> comboNumber5;
    std::optional<NoteStyleAssetData<NoteStyleData_ComboNum>> comboNumber6;
    std::optional<NoteStyleAssetData<NoteStyleData_ComboNum>> comboNumber7;
    std::optional<NoteStyleAssetData<NoteStyleData_ComboNum>> comboNumber8;
    std::optional<NoteStyleAssetData<NoteStyleData_ComboNum>> comboNumber9;

    void from_json(const json& j);
    QString toString() const override;
    QString oneToString(const QString& id) const override {
        return toString();
    }
};

struct NoteStyleData : ISerializable {
    QString version = "1.1.0";
    QString name;
    QString author;
    std::optional<QString> fallback;
    NoteStyleAssetsData assets;

    void from_json(const json& j);
    QString toString() const override;
    QString oneToString(const QString& id) const override {
        return toString();
    }
};

namespace NoteStyleParser {
    std::unique_ptr<NoteStyleData> parseNoteStyleData_VS(const json& j, const QString& filepath = "");
    /**
     * PE的模组系统与官方的差异较大，箭头皮肤引用被定义在songJson里面，UI引用被定义在stageJson里面
     * @param songJson data目录下歌曲的目录中的json文件内容（noteSkin和noteSplash字段所在位置）
     * @param stageJson ui相关配置所在文件，舞台的配置文件
     * @param filepath 调试用
     * @return 符合官方引擎规范的箭头皮肤数据
     */
    std::unique_ptr<NoteStyleData> parseNoteStyleData_PE(const json& songJson,const json& stageJson,const QString& modAbsolutePath, const QString& filepath = "");

    /**
     * 将歌曲 JSON 和舞台 JSON 按 stage 字段配对
     * @param stageJsons 所有舞台 JSON 的 vector
     * @param songJsons 所有歌曲 JSON 的 vector
     * @return 配对的 vector，每个元素是 (stageJson, songJson)
     *         如果某歌曲找不到对应的舞台，舞台位置为null或空 json
     */
    QVector<std::pair<json,json>> buildNoteStageMap(const QVector<json>& stageJsons,const QVector<json>& songJsons,
        const QVector<QString>& stageFilePaths);

    // 获取箭头皮肤名称（带默认值）
    inline QString getNoteSkinsNameOrDefault(const json& songJson, const QString& defaultValue)
    {
        json songData = songJson;
        if (songJson.contains("song") && songJson["song"].is_object()) {
            songData = songJson["song"];
        }

        if (songData.contains("arrowSkin") && songData["arrowSkin"].is_string()) {
            return QString::fromStdString(songData["arrowSkin"].get<std::string>());
        }
        return defaultValue;
    }

    // 获取打击特效名称（带默认值）
    inline QString getNoteSplashNameOrDefault(const json& songJson, const QString& defaultValue)
    {
        json songData = songJson;
        if (songJson.contains("song") && songJson["song"].is_object()) {
            songData = songJson["song"];
        }

        if (songData.contains("splashSkin") && songData["splashSkin"].is_string()) {
            return QString::fromStdString(songData["splashSkin"].get<std::string>());
        }
        return defaultValue;
    }


}