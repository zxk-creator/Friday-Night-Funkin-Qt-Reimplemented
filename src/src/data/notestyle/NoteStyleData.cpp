//
// Created by kkplay on 5/3/26.
//

#include "data/notestyle/NoteStyleData.h"
#include <QDebug>

#include "utils/exception/CustomException.h"
#include "utils/message/MessageHandler.h"

using namespace std;

// ==================== NoteStyleData_Note ====================
void NoteStyleData_Note::from_json(const json& j) {
    if (j.contains("left")) left.from_json(j["left"]);
    if (j.contains("down")) down.from_json(j["down"]);
    if (j.contains("up")) up.from_json(j["up"]);
    if (j.contains("right")) right.from_json(j["right"]);
}

QString NoteStyleData_Note::toString() const {
    QString res = "音符数据:\n";
    res += "  左侧: " + left.toString() + "\n";
    res += "  下侧: " + down.toString() + "\n";
    res += "  上侧: " + up.toString() + "\n";
    res += "  右侧: " + right.toString();
    return res;
}

// ==================== NoteStyleData_Countdown ====================
void NoteStyleData_Countdown::from_json(const json& j) {
    if (j.contains("audioPath")) audioPath = QString::fromStdString(j["audioPath"].get<std::string>());
}

QString NoteStyleData_Countdown::toString() const {
    return "倒计时音效数据:\n  音频路径: " + audioPath;
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

QString NoteStyleData_NoteStrumline::toString() const {
    QString res = "音符轨道线数据:\n";
    res += "  [左侧轨道]\n";
    res += "    静态: " + leftStatic.toString() + "\n";
    res += "    按下: " + leftPress.toString() + "\n";
    res += "    确认: " + leftConfirm.toString() + "\n";
    res += "    确认长按: " + leftConfirmHold.toString() + "\n";
    res += "  [下侧轨道]\n";
    res += "    静态: " + downStatic.toString() + "\n";
    res += "    按下: " + downPress.toString() + "\n";
    res += "    确认: " + downConfirm.toString() + "\n";
    res += "    确认长按: " + downConfirmHold.toString() + "\n";
    res += "  [上侧轨道]\n";
    res += "    静态: " + upStatic.toString() + "\n";
    res += "    按下: " + upPress.toString() + "\n";
    res += "    确认: " + upConfirm.toString() + "\n";
    res += "    确认长按: " + upConfirmHold.toString() + "\n";
    res += "  [右侧轨道]\n";
    res += "    静态: " + rightStatic.toString() + "\n";
    res += "    按下: " + rightPress.toString() + "\n";
    res += "    确认: " + rightConfirm.toString() + "\n";
    res += "    确认长按: " + rightConfirmHold.toString();
    return res;
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

QString NoteStyleData_NoteSplash::toString() const {
    QString res = "音符溅射效果数据:\n";

    if (enabled.has_value()) {
        res += "  启用: " + QString(enabled.value() ? "是" : "否") + "\n";
    }
    if (framerateDefault.has_value()) {
        res += "  默认帧率: " + QString::number(framerateDefault.value()) + "\n";
    }
    if (framerateVariance.has_value()) {
        res += "  帧率变化范围: " + QString::number(framerateVariance.value()) + "\n";
    }
    if (blendMode.has_value()) {
        res += "  混合模式: " + blendMode.value() + "\n";
    }

    if (leftSplashes.has_value()) {
        res += "  左侧溅射动画数量: " + QString::number(leftSplashes->size()) + "\n";
    }
    if (downSplashes.has_value()) {
        res += "  下侧溅射动画数量: " + QString::number(downSplashes->size()) + "\n";
    }
    if (upSplashes.has_value()) {
        res += "  上侧溅射动画数量: " + QString::number(upSplashes->size()) + "\n";
    }
    if (rightSplashes.has_value()) {
        res += "  右侧溅射动画数量: " + QString::number(rightSplashes->size()) + "\n";
    }

    return res;
}

// ==================== NoteStyleData_HoldNoteCoverDirectionData ====================
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

QString NoteStyleData_HoldNoteCoverDirectionData::toString() const {
    QString res = "长按音符覆盖层方向数据:\n";
    if (assetPath.has_value()) {
        res += "  资源路径: " + assetPath.value() + "\n";
    }
    if (start.has_value()) {
        res += "  起始动画: " + start->toString() + "\n";
    }
    if (hold.has_value()) {
        res += "  保持动画: " + hold->toString() + "\n";
    }
    if (end.has_value()) {
        res += "  结束动画: " + end->toString();
    }
    return res;
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

QString NoteStyleData_HoldNoteCover::toString() const {
    QString res = "长按音符覆盖层数据:\n";
    if (enabled.has_value()) {
        res += "  启用: " + QString(enabled.value() ? "是" : "否") + "\n";
    }
    if (left.has_value()) {
        res += "  左侧: " + left->toString() + "\n";
    }
    if (down.has_value()) {
        res += "  下侧: " + down->toString() + "\n";
    }
    if (up.has_value()) {
        res += "  上侧: " + up->toString() + "\n";
    }
    if (right.has_value()) {
        res += "  右侧: " + right->toString();
    }
    return res;
}

// ==================== NoteStyleData_ComboNum ====================
void NoteStyleData_ComboNum::from_json(const json&) {}

QString NoteStyleData_ComboNum::toString() const {
    return "连击数字数据: 空(预留)";
}

// ==================== NoteStyleData_Judgement ====================
void NoteStyleData_Judgement::from_json(const json&) {}

QString NoteStyleData_Judgement::toString() const {
    return "判定文字数据: 空(预留)";
}

// ==================== NoteStyleAssetData 模板实现 ====================
template<typename T>
QString NoteStyleAssetData<T>::toString() const {
    QString res = "资源数据:\n";
    res += "  资源路径: " + assetPath + "\n";
    res += "  缩放: " + QString::number(scale) + "\n";
    res += "  偏移量: [" + QString::number(offsets[0]) + ", " + QString::number(offsets[1]) + "]\n";
    res += "  是否像素风格: " + QString(isPixel ? "是" : "否") + "\n";
    res += "  透明度: " + QString::number(alpha) + "\n";
    res += "  是否动画: " + QString(animated ? "是" : "否") + "\n";
    if (data.has_value()) {
        res += "  附加数据:\n" + data->toString();
    }
    return res;
}

template struct NoteStyleAssetData<NoteStyleData_Note>;
template struct NoteStyleAssetData<NoteStyleData_HoldNote>;
template struct NoteStyleAssetData<NoteStyleData_NoteStrumline>;
template struct NoteStyleAssetData<NoteStyleData_NoteSplash>;
template struct NoteStyleAssetData<NoteStyleData_HoldNoteCover>;
template struct NoteStyleAssetData<NoteStyleData_Countdown>;
template struct NoteStyleAssetData<NoteStyleData_Judgement>;
template struct NoteStyleAssetData<NoteStyleData_ComboNum>;

// ==================== NoteStyleAssetsData ====================
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

QString NoteStyleAssetsData::toString() const {
    QString res = "音符样式资源集合:\n";

    #define CHECK_OPTIONAL(opt, name) \
        if (opt.has_value()) { \
            res += "  " name ":\n"; \
            res += opt->toString() + "\n"; \
        }

    CHECK_OPTIONAL(note, "音符");
    CHECK_OPTIONAL(holdNote, "长按音符");
    CHECK_OPTIONAL(noteStrumline, "音符轨道线");
    CHECK_OPTIONAL(noteSplash, "音符溅射效果");
    CHECK_OPTIONAL(holdNoteCover, "长按音符覆盖层");
    CHECK_OPTIONAL(countdownThree, "倒计时3");
    CHECK_OPTIONAL(countdownTwo, "倒计时2");
    CHECK_OPTIONAL(countdownOne, "倒计时1");
    CHECK_OPTIONAL(countdownGo, "倒计时开始");
    CHECK_OPTIONAL(judgementSick, "完美判定");
    CHECK_OPTIONAL(judgementGood, "良好判定");
    CHECK_OPTIONAL(judgementBad, "一般判定");
    CHECK_OPTIONAL(judgementShit, "糟糕判定");
    CHECK_OPTIONAL(comboNumber0, "数字0");
    CHECK_OPTIONAL(comboNumber1, "数字1");
    CHECK_OPTIONAL(comboNumber2, "数字2");
    CHECK_OPTIONAL(comboNumber3, "数字3");
    CHECK_OPTIONAL(comboNumber4, "数字4");
    CHECK_OPTIONAL(comboNumber5, "数字5");
    CHECK_OPTIONAL(comboNumber6, "数字6");
    CHECK_OPTIONAL(comboNumber7, "数字7");
    CHECK_OPTIONAL(comboNumber8, "数字8");
    CHECK_OPTIONAL(comboNumber9, "数字9");

    #undef CHECK_OPTIONAL

    return res;
}

// ==================== NoteStyleData ====================
void NoteStyleData::from_json(const json& j) {
    if (j.contains("version")) version = QString::fromStdString(j["version"].get<std::string>());
    if (j.contains("name")) name = QString::fromStdString(j["name"].get<std::string>());
    if (j.contains("author")) author = QString::fromStdString(j["author"].get<std::string>());
    if (j.contains("fallback")) fallback = QString::fromStdString(j["fallback"].get<std::string>());
    if (j.contains("assets")) assets.from_json(j["assets"]);
}

QString NoteStyleData::toString() const {
    QString res = "========== 音符样式数据 ==========\n";
    res += "版本: " + version + "\n";
    res += "名称: " + name + "\n";
    res += "作者: " + author + "\n";
    if (fallback.has_value()) {
        res += "备用样式: " + fallback.value() + "\n";
    }
    res += "\n" + assets.toString();
    return res;
}

// ==================== 解析函数 ====================
unique_ptr<NoteStyleData> parseNoteStyleData(const json& j, const QString& filepath) {
    try {
        auto data = make_unique<NoteStyleData>();
        data->from_json(j);

        if (data->name.isEmpty()) {
            LOG_WARNING("在" + filepath + "中缺少name字段");
            return nullptr;
        }
        if (data->author.isEmpty()) {
            LOG_WARNING("在" + filepath + "中缺少author字段");
            return nullptr;
        }

        return data;
    } catch (const std::exception& e) {
        LOG_JSON_PARSE_ERROR(e.what(),filepath);
        return nullptr;
    }
}