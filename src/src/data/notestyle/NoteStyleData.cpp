//
// Created by kkplay on 5/3/26.
//

#include "data/notestyle/NoteStyleData.h"
#include <QDebug>

// ==================== NoteStyleData_Note ====================
void NoteStyleData_Note::from_json(const json& j) {
    if (j.contains("left")) left.from_json(j["left"]);
    if (j.contains("down")) down.from_json(j["down"]);
    if (j.contains("up")) up.from_json(j["up"]);
    if (j.contains("right")) right.from_json(j["right"]);
}


// ==================== NoteStyleData_Countdown ====================
void NoteStyleData_Countdown::from_json(const json& j) {
    if (j.contains("audioPath")) audioPath = QString::fromStdString(j["audioPath"].get<std::string>());
}

// ==================== NoteStyleData_NoteStrumline ====================
void NoteStyleData_NoteStrumline::from_json(const json& j) {
    if (j.contains("leftStatic")) leftStatic.from_json(j["leftStatic"]);
    if (j.contains("leftPress")) leftPress.from_json(j["leftPress"]);
    if (j.contains("leftConfirm")) leftConfirm.from_json(j["leftConfirm"]);
    if (j.contains("leftConfirmHold")) leftConfirmHold.from_json(j["leftConfirmHold"]);
    if (j.contains("downStatic")) downStatic.from_json(j["downStatic"]);
    if (j.contains("downPress")) downPress.from_json(j["downPress"]);
    if (j.contains("downConfirm")) downConfirm.from_json(j["downConfirm"]);
    if (j.contains("downConfirmHold")) downConfirmHold.from_json(j["downConfirmHold"]);
    if (j.contains("upStatic")) upStatic.from_json(j["upStatic"]);
    if (j.contains("upPress")) upPress.from_json(j["upPress"]);
    if (j.contains("upConfirm")) upConfirm.from_json(j["upConfirm"]);
    if (j.contains("upConfirmHold")) upConfirmHold.from_json(j["upConfirmHold"]);
    if (j.contains("rightStatic")) rightStatic.from_json(j["rightStatic"]);
    if (j.contains("rightPress")) rightPress.from_json(j["rightPress"]);
    if (j.contains("rightConfirm")) rightConfirm.from_json(j["rightConfirm"]);
    if (j.contains("rightConfirmHold")) rightConfirmHold.from_json(j["rightConfirmHold"]);
}


// ==================== NoteStyleData_NoteSplash ====================
void NoteStyleData_NoteSplash::from_json(const json& j) {
    if (j.contains("enabled")) enabled = j["enabled"].get<bool>();
    if (j.contains("framerateDefault")) framerateDefault = j["framerateDefault"].get<int>();
    if (j.contains("framerateVariance")) framerateVariance = j["framerateVariance"].get<int>();
    if (j.contains("blendMode")) blendMode = QString::fromStdString(j["blendMode"].get<std::string>());

    auto parseSplashes = [](const json& arr) -> QVector<UnnamedAnimationData> {
        QVector<UnnamedAnimationData> result;
        for (const auto& item : arr) {
            UnnamedAnimationData anim;
            anim.from_json(item);
            result.push_back(anim);
        }
        return result;
    };

    if (j.contains("leftSplashes")) leftSplashes = parseSplashes(j["leftSplashes"]);
    if (j.contains("downSplashes")) downSplashes = parseSplashes(j["downSplashes"]);
    if (j.contains("upSplashes")) upSplashes = parseSplashes(j["upSplashes"]);
    if (j.contains("rightSplashes")) rightSplashes = parseSplashes(j["rightSplashes"]);
}

void NoteStyleData_HoldNoteCoverDirectionData::from_json(const json& j) {
    if (j.contains("assetPath")) assetPath = QString::fromStdString(j["assetPath"].get<std::string>());
    if (j.contains("start")) {
        start = UnnamedAnimationData();
        start->from_json(j["start"]);
    }
    if (j.contains("hold")) {
        hold = UnnamedAnimationData();
        hold->from_json(j["hold"]);
    }
    if (j.contains("end")) {
        end = UnnamedAnimationData();
        end->from_json(j["end"]);
    }
}

// ==================== NoteStyleData_HoldNoteCover ====================
void NoteStyleData_HoldNoteCover::from_json(const json& j) {
    if (j.contains("enabled")) enabled = j["enabled"].get<bool>();

    auto parseDirection = [](const json& val) -> NoteStyleData_HoldNoteCoverDirectionData {
        NoteStyleData_HoldNoteCoverDirectionData dir;
        dir.from_json(val);
        return dir;
    };

    if (j.contains("left")) left = parseDirection(j["left"]);
    if (j.contains("down")) down = parseDirection(j["down"]);
    if (j.contains("up")) up = parseDirection(j["up"]);
    if (j.contains("right")) right = parseDirection(j["right"]);
}

void NoteStyleData_ComboNum::from_json(const json&) {}

void NoteStyleData_Judgement::from_json(const json&) {}

void NoteStyleAssetsData::from_json(const json& j) {

    auto parseAsset = [&](const char* key, auto& opt, auto& defaultValue) {
        if (j.contains(key) && !j[key].is_null()) {
            // 空的结构体，我也不解析了！！！
            opt = defaultValue;
        }
    };

    NoteStyleAssetData<NoteStyleData_Note> tempNote;
    parseAsset("note", note, tempNote);

    NoteStyleAssetData<NoteStyleData_HoldNote> tempHoldNote;
    parseAsset("holdNote", holdNote, tempHoldNote);

    NoteStyleAssetData<NoteStyleData_NoteStrumline> tempStrumline;
    parseAsset("noteStrumline", noteStrumline, tempStrumline);

    NoteStyleAssetData<NoteStyleData_NoteSplash> tempSplash;
    parseAsset("noteSplash", noteSplash, tempSplash);

    NoteStyleAssetData<NoteStyleData_HoldNoteCover> tempCover;
    parseAsset("holdNoteCover", holdNoteCover, tempCover);

    NoteStyleAssetData<NoteStyleData_Countdown> tempCountdown;
    parseAsset("countdownThree", countdownThree, tempCountdown);
    parseAsset("countdownTwo", countdownTwo, tempCountdown);
    parseAsset("countdownOne", countdownOne, tempCountdown);
    parseAsset("countdownGo", countdownGo, tempCountdown);

    NoteStyleAssetData<NoteStyleData_Judgement> tempJudgement;
    parseAsset("judgementSick", judgementSick, tempJudgement);
    parseAsset("judgementGood", judgementGood, tempJudgement);
    parseAsset("judgementBad", judgementBad, tempJudgement);
    parseAsset("judgementShit", judgementShit, tempJudgement);

    NoteStyleAssetData<NoteStyleData_ComboNum> tempCombo;
    parseAsset("comboNumber0", comboNumber0, tempCombo);
    parseAsset("comboNumber1", comboNumber1, tempCombo);
    parseAsset("comboNumber2", comboNumber2, tempCombo);
    parseAsset("comboNumber3", comboNumber3, tempCombo);
    parseAsset("comboNumber4", comboNumber4, tempCombo);
    parseAsset("comboNumber5", comboNumber5, tempCombo);
    parseAsset("comboNumber6", comboNumber6, tempCombo);
    parseAsset("comboNumber7", comboNumber7, tempCombo);
    parseAsset("comboNumber8", comboNumber8, tempCombo);
    parseAsset("comboNumber9", comboNumber9, tempCombo);
}


void NoteStyleData::from_json(const json& j) {
    if (j.contains("version")) version = QString::fromStdString(j["version"].get<std::string>());
    if (j.contains("name")) name = QString::fromStdString(j["name"].get<std::string>());
    if (j.contains("author")) author = QString::fromStdString(j["author"].get<std::string>());
    if (j.contains("fallback")) fallback = QString::fromStdString(j["fallback"].get<std::string>());
    if (j.contains("assets")) assets.from_json(j["assets"]);
}

// ==================== 解析函数 ====================
std::optional<NoteStyleData> parseNoteStyleData(const json& j, const QString& filepath) {
    try {
        NoteStyleData data;
        data.from_json(j);

        if (data.name.isEmpty()) {
            qDebug() << "ERROR: Missing name in note style file:" << filepath;
            return std::nullopt;
        }
        if (data.author.isEmpty()) {
            qDebug() << "ERROR: Missing author in note style file:" << filepath;
            return std::nullopt;
        }

        return data;
    } catch (const std::exception& e) {
        qDebug() << "Failed to parse note style data from" << filepath << ":" << e.what();
        return std::nullopt;
    }
}
