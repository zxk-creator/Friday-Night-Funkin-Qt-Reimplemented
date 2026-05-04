//
// Created by kkplay on 5/3/26.
//

#pragma once

#include <QVector>
#include <QHash>
#include <optional>
#include "data/animation/AnimationData.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

enum class NoteDirection {
    Left,
    Down,
    Up,
    Right
};


struct NoteStyleData_Note {
    UnnamedAnimationData left;
    UnnamedAnimationData down;
    UnnamedAnimationData up;
    UnnamedAnimationData right;

    void from_json(const json& j);
};

struct NoteStyleData_Countdown {
    QString audioPath;

    void from_json(const json& j);
};


struct NoteStyleData_NoteStrumline {
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
};

struct NoteStyleData_NoteSplash {
    std::optional<bool> enabled = true;
    std::optional<int> framerateDefault = 24;
    std::optional<int> framerateVariance = 2;
    std::optional<QString> blendMode = "normal";
    std::optional<QVector<UnnamedAnimationData>> leftSplashes;
    std::optional<QVector<UnnamedAnimationData>> downSplashes;
    std::optional<QVector<UnnamedAnimationData>> upSplashes;
    std::optional<QVector<UnnamedAnimationData>> rightSplashes;

    void from_json(const json& j);
};

struct NoteStyleData_HoldNoteCoverDirectionData {
    std::optional<QString> assetPath;
    std::optional<UnnamedAnimationData> start;
    std::optional<UnnamedAnimationData> hold;
    std::optional<UnnamedAnimationData> end;

    void from_json(const json& j);
};


struct NoteStyleData_HoldNoteCover {
    std::optional<bool> enabled = true;
    std::optional<NoteStyleData_HoldNoteCoverDirectionData> left;
    std::optional<NoteStyleData_HoldNoteCoverDirectionData> down;
    std::optional<NoteStyleData_HoldNoteCoverDirectionData> up;
    std::optional<NoteStyleData_HoldNoteCoverDirectionData> right;

    void from_json(const json& j);
};


struct NoteStyleData_ComboNum {
    // 目前没有字段，预留
    void from_json(const json& j);
};


struct NoteStyleData_Judgement {
    // 目前没有字段，预留
    void from_json(const json& j);
};


template<typename T>
struct NoteStyleAssetData {
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
};

// 这个haxe源码并没有定义，所以我也不定义了
struct NoteStyleData_HoldNote {
};


/**
 * 通用的资源描述结构，用于描述音符样式系统中任何资源的通用属性。
 */
struct NoteStyleAssetsData {
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
};
// haxe写起来是爽了，可是用C++的难受了

struct NoteStyleData {
    QString version = "1.1.0";
    QString name;
    QString author;
    std::optional<QString> fallback;
    NoteStyleAssetsData assets;

    void from_json(const json& j);
};

std::optional<NoteStyleData> parseNoteStyleData(const json& j, const QString& filepath = "");