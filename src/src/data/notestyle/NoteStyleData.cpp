//
// Created by kkplay on 5/3/26.
//

#include "data/notestyle/NoteStyleData.h"
#include <QDebug>

#include "Constants.h"
#include "data/Context.h"
#include "utils/exception/CustomException.h"
#include "utils/file/FileUtil.h"
#include "utils/lang/LangStringPool.h"
#include "utils/message/MessageHandler.h"

using namespace std;

// ==================== NoteStyleData_Note ====================
void NoteStyleData_Note::from_json(const json& j)
{
    if (j.contains("left")) left.from_json(j["left"]);
    if (j.contains("down")) down.from_json(j["down"]);
    if (j.contains("up")) up.from_json(j["up"]);
    if (j.contains("right")) right.from_json(j["right"]);
}

QString NoteStyleData_Note::toString() const
{
    QString res = "音符数据:\n";
    res += "  左侧: " + left.toString() + "\n";
    res += "  下侧: " + down.toString() + "\n";
    res += "  上侧: " + up.toString() + "\n";
    res += "  右侧: " + right.toString();
    return res;
}

// ==================== NoteStyleData_Countdown ====================
void NoteStyleData_Countdown::from_json(const json& j)
{
    if (j.contains("audioPath")) audioPath = QString::fromStdString(j["audioPath"].get<std::string>());
}

QString NoteStyleData_Countdown::toString() const
{
    return "倒计时音效数据:\n  音频路径: " + audioPath;
}

// ==================== NoteStyleData_NoteStrumline ====================
void NoteStyleData_NoteStrumline::from_json(const json& j)
{
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

QString NoteStyleData_NoteStrumline::toString() const
{
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
void NoteStyleData_NoteSplash::from_json(const json& j)
{
    if (j.contains("enabled")) enabled = j["enabled"].get<bool>();
    if (j.contains("framerateDefault")) framerateDefault = j["framerateDefault"].get<int>();
    if (j.contains("framerateVariance")) framerateVariance = j["framerateVariance"].get<int>();
    if (j.contains("blendMode")) blendMode = QString::fromStdString(j["blendMode"].get<std::string>());

    auto parseSplashes = [](const json& arr) -> QVector<UnnamedAnimationData>
    {
        QVector<UnnamedAnimationData> result;
        for (const auto& item : arr)
        {
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

QString NoteStyleData_NoteSplash::toString() const
{
    QString res = "音符溅射效果数据:\n";

    if (enabled.has_value())
    {
        res += "  启用: " + QString(enabled.value() ? "是" : "否") + "\n";
    }
    if (framerateDefault.has_value())
    {
        res += "  默认帧率: " + QString::number(framerateDefault.value()) + "\n";
    }
    if (framerateVariance.has_value())
    {
        res += "  帧率变化范围: " + QString::number(framerateVariance.value()) + "\n";
    }
    if (blendMode.has_value())
    {
        res += "  混合模式: " + blendMode.value() + "\n";
    }

    if (leftSplashes.has_value())
    {
        res += "  左侧溅射动画数量: " + QString::number(leftSplashes->size()) + "\n";
    }
    if (downSplashes.has_value())
    {
        res += "  下侧溅射动画数量: " + QString::number(downSplashes->size()) + "\n";
    }
    if (upSplashes.has_value())
    {
        res += "  上侧溅射动画数量: " + QString::number(upSplashes->size()) + "\n";
    }
    if (rightSplashes.has_value())
    {
        res += "  右侧溅射动画数量: " + QString::number(rightSplashes->size()) + "\n";
    }

    return res;
}

// ==================== NoteStyleData_HoldNoteCoverDirectionData ====================
void NoteStyleData_HoldNoteCoverDirectionData::from_json(const json& j)
{
    if (j.contains("assetPath")) assetPath = QString::fromStdString(j["assetPath"].get<std::string>());
    if (j.contains("start"))
    {
        start = UnnamedAnimationData();
        start->from_json(j["start"]);
    }
    if (j.contains("hold"))
    {
        hold = UnnamedAnimationData();
        hold->from_json(j["hold"]);
    }
    if (j.contains("end"))
    {
        end = UnnamedAnimationData();
        end->from_json(j["end"]);
    }
}

QString NoteStyleData_HoldNoteCoverDirectionData::toString() const
{
    QString res = "长按音符覆盖层方向数据:\n";
    if (assetPath.has_value())
    {
        res += "  资源路径: " + assetPath.value() + "\n";
    }
    if (start.has_value())
    {
        res += "  起始动画: " + start->toString() + "\n";
    }
    if (hold.has_value())
    {
        res += "  保持动画: " + hold->toString() + "\n";
    }
    if (end.has_value())
    {
        res += "  结束动画: " + end->toString();
    }
    return res;
}

// ==================== NoteStyleData_HoldNoteCover ====================
void NoteStyleData_HoldNoteCover::from_json(const json& j)
{
    if (j.contains("enabled")) enabled = j["enabled"].get<bool>();

    auto parseDirection = [](const json& val) -> NoteStyleData_HoldNoteCoverDirectionData
    {
        NoteStyleData_HoldNoteCoverDirectionData dir;
        dir.from_json(val);
        return dir;
    };

    if (j.contains("left")) left = parseDirection(j["left"]);
    if (j.contains("down")) down = parseDirection(j["down"]);
    if (j.contains("up")) up = parseDirection(j["up"]);
    if (j.contains("right")) right = parseDirection(j["right"]);
}

QString NoteStyleData_HoldNoteCover::toString() const
{
    QString res = "长按音符覆盖层数据:\n";
    if (enabled.has_value())
    {
        res += "  启用: " + QString(enabled.value() ? "是" : "否") + "\n";
    }
    if (left.has_value())
    {
        res += "  左侧: " + left->toString() + "\n";
    }
    if (down.has_value())
    {
        res += "  下侧: " + down->toString() + "\n";
    }
    if (up.has_value())
    {
        res += "  上侧: " + up->toString() + "\n";
    }
    if (right.has_value())
    {
        res += "  右侧: " + right->toString();
    }
    return res;
}

// ==================== NoteStyleData_ComboNum ====================
void NoteStyleData_ComboNum::from_json(const json&)
{
}

QString NoteStyleData_ComboNum::toString() const
{
    return "连击数字数据: 空(预留)";
}

// ==================== NoteStyleData_Judgement ====================
void NoteStyleData_Judgement::from_json(const json&)
{
}

QString NoteStyleData_Judgement::toString() const
{
    return "判定文字数据: 空(预留)";
}

// ==================== NoteStyleAssetData 模板实现 ====================
template <typename T>
QString NoteStyleAssetData<T>::toString() const
{
    QString res = "资源数据:\n";
    res += "  资源路径: " + assetPath + "\n";
    res += "  缩放: " + QString::number(scale) + "\n";
    res += "  偏移量: [" + QString::number(offsets[0]) + ", " + QString::number(offsets[1]) + "]\n";
    res += "  是否像素风格: " + QString(isPixel ? "是" : "否") + "\n";
    res += "  透明度: " + QString::number(alpha) + "\n";
    res += "  是否动画: " + QString(animated ? "是" : "否") + "\n";
    if (data.has_value())
    {
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
void NoteStyleAssetsData::from_json(const json& j)
{
    auto parseAsset = [&](const char* key, auto& opt, auto& defaultValue)
    {
        if (j.contains(key) && !j[key].is_null())
        {
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

QString NoteStyleAssetsData::toString() const
{
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
void NoteStyleData::from_json(const json& j)
{
    if (j.contains("version")) version = QString::fromStdString(j["version"].get<std::string>());
    if (j.contains("name")) name = QString::fromStdString(j["name"].get<std::string>());
    if (j.contains("author")) author = QString::fromStdString(j["author"].get<std::string>());
    if (j.contains("fallback")) fallback = QString::fromStdString(j["fallback"].get<std::string>());
    if (j.contains("assets")) assets.from_json(j["assets"]);
}

QString NoteStyleData::toString() const
{
    QString res = "========== 音符样式数据 ==========\n";
    res += "版本: " + version + "\n";
    res += "名称: " + name + "\n";
    res += "作者: " + author + "\n";
    if (fallback.has_value())
    {
        res += "备用样式: " + fallback.value() + "\n";
    }
    res += "\n" + assets.toString();
    return res;
}

unique_ptr<NoteStyleData> NoteStyleParser::parseNoteStyleData_VS(const json& j, const QString& filepath)
{
    try
    {
        auto data = make_unique<NoteStyleData>();
        data->from_json(j);

        if (data->name.isEmpty())
        {
            LOG_WARNING(false, "在" + filepath + "中缺少name字段");
            return nullptr;
        }
        if (data->author.isEmpty())
        {
            LOG_WARNING(false, "在" + filepath + "中缺少author字段");
            return nullptr;
        }

        return data;
    }
    catch (const std::exception& e)
    {
        LOG_JSON_PARSE_ERROR(false, e.what(), filepath);
        return nullptr;
    }
}


// 一些辅助解析函数！
// 创建基础动画
UnnamedAnimationData createAnimation(const QString& prefix, bool looped)
{
    UnnamedAnimationData anim;
    anim.prefix = prefix;
    anim.offsets = {0, 0};
    anim.looped = looped;
    anim.flipX = false;
    anim.flipY = false;
    anim.frameRate = 24;
    anim.animType = "framelabel";
    return anim;
}

// 设置轨道方向数据
void setStrumlineDirection(NoteStyleData_NoteStrumline& strumline, const QString& direction,
                           const UnnamedAnimationData& staticAnim, const UnnamedAnimationData& pressAnim,
                           const UnnamedAnimationData& confirmAnim, const UnnamedAnimationData& confirmHoldAnim)
{
    if (direction == "left")
    {
        strumline.leftStatic = staticAnim;
        strumline.leftPress = pressAnim;
        strumline.leftConfirm = confirmAnim;
        strumline.leftConfirmHold = confirmHoldAnim;
    }
    else if (direction == "down")
    {
        strumline.downStatic = staticAnim;
        strumline.downPress = pressAnim;
        strumline.downConfirm = confirmAnim;
        strumline.downConfirmHold = confirmHoldAnim;
    }
    else if (direction == "up")
    {
        strumline.upStatic = staticAnim;
        strumline.upPress = pressAnim;
        strumline.upConfirm = confirmAnim;
        strumline.upConfirmHold = confirmHoldAnim;
    }
    else if (direction == "right")
    {
        strumline.rightStatic = staticAnim;
        strumline.rightPress = pressAnim;
        strumline.rightConfirm = confirmAnim;
        strumline.rightConfirmHold = confirmHoldAnim;
    }
}

// 创建轨道资产
NoteStyleAssetData<NoteStyleData_NoteStrumline> createStrumlineAsset(const QString& skinPath)
{
    NoteStyleAssetData<NoteStyleData_NoteStrumline> asset;
    asset.animated = true;
    asset.alpha = 1.0f;
    asset.assetPath = skinPath;
    asset.scale = 1.0f;
    asset.isPixel = false;

    NoteStyleData_NoteStrumline strumline;
    QStringList colors = {"purple", "blue", "green", "red"};
    QStringList directions = {"left", "down", "up", "right"};

    for (int i = 0; i < colors.size(); i++)
    {
        const QString& color = colors[i];
        const QString& direction = directions[i];

        auto staticAnim = createAnimation(color + " static", true);
        auto pressAnim = createAnimation(color + " press", false);
        auto confirmAnim = createAnimation(color + " confirm", false);
        auto confirmHoldAnim = createAnimation(color + " confirm hold", true);

        setStrumlineDirection(strumline, direction, staticAnim, pressAnim, confirmAnim, confirmHoldAnim);
    }

    asset.data = strumline;
    return asset;
}

// 创建打击特效资产
NoteStyleAssetData<NoteStyleData_NoteSplash> createSplashAsset(const QString& splashPath)
{
    NoteStyleAssetData<NoteStyleData_NoteSplash> asset;
    asset.animated = true;
    asset.alpha = 1.0f;
    asset.assetPath = splashPath;
    asset.scale = 1.0f;
    asset.isPixel = false;

    NoteStyleData_NoteSplash splashData;
    splashData.enabled = true;
    splashData.framerateDefault = 24;
    splashData.framerateVariance = 2;
    splashData.blendMode = "normal";

    asset.data = splashData;
    return asset;
}

// 创建倒计时资产
NoteStyleAssetData<NoteStyleData_Countdown> createCountdownAsset(const QString& uiPrefix, const QString& name, const QString& uiPostfix)
{
    NoteStyleAssetData<NoteStyleData_Countdown> asset;
    asset.assetPath = uiPrefix + "UI/" + name + uiPostfix;
    asset.animated = true;
    return asset;
}

// 创建判定文字资产
NoteStyleAssetData<NoteStyleData_Judgement> createJudgementAsset(const QString& judgementName)
{
    NoteStyleAssetData<NoteStyleData_Judgement> asset;

    // 判定文字图片路径：shared:ui/judgements/{name}
    asset.assetPath = QString("shared:ui/judgements/%1").arg(judgementName);
    asset.animated = false;      // 判定文字通常是静态图片
    asset.alpha = 1.0f;
    asset.scale = 1.0f;
    asset.isPixel = false;

    // 判定文字没有额外的 data 字段
    // data 保持为 nullopt

    return asset;
}

// 设置倒计时音频（PE 使用硬编码路径）
void setCountdownAudio(std::unique_ptr<NoteStyleData>& style)
{
    // 检查 data 是否存在，如果不存在则创建
    if (!style->assets.countdownThree->data.has_value())
    {
        style->assets.countdownThree->data = NoteStyleData_Countdown();
    }
    style->assets.countdownThree->data->audioPath = "shared:sounds/countdown3";

    if (!style->assets.countdownTwo->data.has_value())
    {
        style->assets.countdownTwo->data = NoteStyleData_Countdown();
    }
    style->assets.countdownTwo->data->audioPath = "shared:sounds/countdown2";

    if (!style->assets.countdownOne->data.has_value())
    {
        style->assets.countdownOne->data = NoteStyleData_Countdown();
    }
    style->assets.countdownOne->data->audioPath = "shared:sounds/countdown1";

    if (!style->assets.countdownGo->data.has_value())
    {
        style->assets.countdownGo->data = NoteStyleData_Countdown();
    }
    style->assets.countdownGo->data->audioPath = "shared:sounds/countdownGo";
}

std::unique_ptr<NoteStyleData> NoteStyleParser::parseNoteStyleData_PE(
    const json& songJson,
    const json& stageJson,
    const QString& modAbsolutePath,
    const QString& filepath)
{
    auto style = std::make_unique<NoteStyleData>();

    try
    {
        // ========== 1. 基础信息 ==========
        style->version = NoteStyleDataRelative::NOTE_STYLE_DATA_VERSION;
        style->name = "Psych Engine Style";
        style->author = Context::lang->unknownAuthor();
        style->fallback = "funkin";  // 回退到官方默认

        // ========== 2. 解析 JSON 数据 ==========
        json songData = songJson;
        if (songJson.contains("song") && songJson["song"].is_object())
        {
            songData = songJson["song"];
        }

        // 默认路径
        QString arrowSkin = "noteSkins/NOTE_assets";
        QString splashSkin = "noteSplashes/noteSplashes";

        // ========== 3. 解析舞台 UI 配置 ==========
        QString stageUI = "normal";
        bool isPixelStage = false;

        if (stageJson.contains("stageUI") && stageJson["stageUI"].is_string())
        {
            stageUI = QString::fromStdString(stageJson["stageUI"].get<std::string>());
        }
        if (stageJson.contains("isPixelStage") && stageJson["isPixelStage"].is_boolean())
        {
            isPixelStage = stageJson["isPixelStage"].get<bool>();
        }

        // 构建 UI 前缀和后缀
        QString uiPrefix = "";
        QString uiPostfix = "";
        if (stageUI == "pixel" || stageUI.endsWith("-pixel"))
        {
            uiPostfix = "-pixel";
            uiPrefix = stageUI.left(stageUI.length() - 6);
        }
        else if (stageUI != "normal")
        {
            uiPrefix = stageUI;
        }

        // ========== 4. 初始化默认资产 ==========
        // 音符资产
        style->assets.note = NoteStyleAssetData<NoteStyleData_Note>();
        style->assets.note->assetPath = arrowSkin;
        style->assets.note->animated = true;
        style->assets.note->scale = 1.0f;
        style->assets.note->isPixel = isPixelStage;

        // 轨道资产
        style->assets.noteStrumline = NoteStyleAssetData<NoteStyleData_NoteStrumline>();
        style->assets.noteStrumline->assetPath = arrowSkin;
        style->assets.noteStrumline->animated = true;
        style->assets.noteStrumline->isPixel = isPixelStage;

        // 打击特效资产
        style->assets.noteSplash = NoteStyleAssetData<NoteStyleData_NoteSplash>();
        style->assets.noteSplash->assetPath = splashSkin;
        style->assets.noteSplash->animated = true;
        style->assets.noteSplash->isPixel = isPixelStage;

        // ========== 5. 解析箭头皮肤 ==========
        bool foundArrowSkin = false;

        if (songData.contains("arrowSkin") && songData["arrowSkin"].is_string())
        {
            arrowSkin = QString::fromStdString(songData["arrowSkin"].get<std::string>());
            QString noteId = FileUtil::fetchIdFromFileName(arrowSkin);
            QString fullSkinPath = QString("NOTE_assets-%1").arg(noteId.isEmpty() ? "default" : noteId);

            if (PathPE::image(modAbsolutePath, fullSkinPath).has_value())
            {
                foundArrowSkin = true;

                // 音符资产
                NoteStyleAssetData<NoteStyleData_Note> noteAsset;
                noteAsset.animated = true;
                noteAsset.alpha = 1.0f;
                noteAsset.assetPath = fullSkinPath;
                noteAsset.scale = 1.0f;
                noteAsset.isPixel = false;
                style->assets.note = noteAsset;

                // 轨道资产（带完整动画配置）
                auto strumlineAsset = createStrumlineAsset(fullSkinPath);
                style->assets.noteStrumline = strumlineAsset;

                // 长按音符资产
                NoteStyleAssetData<NoteStyleData_HoldNote> holdNoteAsset;
                holdNoteAsset.animated = true;
                holdNoteAsset.assetPath = fullSkinPath;
                holdNoteAsset.scale = 1.0f;
                style->assets.holdNote = holdNoteAsset;

                LOG_INFO("发现自定义箭头皮肤: " + fullSkinPath);
            }
            else
            {
                LOG_INFO("未找到自定义箭头皮肤: " + fullSkinPath);
            }
        }

        // 尝试默认皮肤
        if (!foundArrowSkin)
        {
            QString defaultSkinPath = "NOTE_assets";
            if (PathPE::image(modAbsolutePath, defaultSkinPath).has_value())
            {
                auto strumlineAsset = createStrumlineAsset(defaultSkinPath);
                style->assets.noteStrumline = strumlineAsset;

                NoteStyleAssetData<NoteStyleData_Note> noteAsset;
                noteAsset.animated = true;
                noteAsset.assetPath = defaultSkinPath;
                noteAsset.scale = 1.0f;
                style->assets.note = noteAsset;

                NoteStyleAssetData<NoteStyleData_HoldNote> holdNoteAsset;
                holdNoteAsset.animated = true;
                holdNoteAsset.assetPath = defaultSkinPath;
                holdNoteAsset.scale = 1.0f;
                style->assets.holdNote = holdNoteAsset;

                LOG_INFO("使用默认箭头皮肤: " + defaultSkinPath);
            }
            else
            {
                LOG_INFO("未找到任何箭头皮肤，将使用引擎内置默认值");
            }
        }

        // ========== 6. 解析打击特效皮肤 ==========
        bool foundSplash = false;

        if (songData.contains("splashSkin") && songData["splashSkin"].is_string())
        {
            splashSkin = QString::fromStdString(songData["splashSkin"].get<std::string>());
            QString splashId = FileUtil::fetchIdFromFileName(splashSkin);
            QString fullSplashPath = QString("%1-%2").arg(splashSkin).arg(splashId.isEmpty() ? "default" : splashId);

            if (PathPE::image(modAbsolutePath, fullSplashPath).has_value())
            {
                foundSplash = true;
                style->assets.noteSplash = createSplashAsset(fullSplashPath);
                LOG_INFO("发现自定义打击特效: " + fullSplashPath);
            }
            else
            {
                LOG_INFO("未找到自定义打击特效: " + fullSplashPath);
            }
        }

        // 尝试默认打击特效
        if (!foundSplash)
        {
            QString defaultSplashPath = "noteSplashes/noteSplashes";
            if (PathPE::image(modAbsolutePath, defaultSplashPath).has_value())
            {
                style->assets.noteSplash = createSplashAsset(defaultSplashPath);
                LOG_INFO("使用默认打击特效: " + defaultSplashPath);
            }
        }

        // ========== 7. 倒计时资产 ==========
        style->assets.countdownThree = createCountdownAsset(uiPrefix, "ready", uiPostfix);
        style->assets.countdownTwo = createCountdownAsset(uiPrefix, "set", uiPostfix);
        style->assets.countdownOne = createCountdownAsset(uiPrefix, "go", uiPostfix);
        style->assets.countdownGo = createCountdownAsset(uiPrefix, "go", uiPostfix);

        setCountdownAudio(style);

        // ========== 8. 判定文字资产 ==========
        style->assets.judgementSick = createJudgementAsset("sick");
        style->assets.judgementGood = createJudgementAsset("good");
        style->assets.judgementBad = createJudgementAsset("bad");
        style->assets.judgementShit = createJudgementAsset("shit");

        // ========== 9. 组合数字资产 ==========
        for (int i = 0; i <= 9; i++)
        {

        }

        // ========== 10. 长按覆盖物（PE 不使用，保持为空） ==========
        // style->assets.holdNoteCover 保持为 nullopt

        LOG_INFO("音符样式解析完成");
    }
    catch (const std::exception& e)
    {
        LOG_JSON_PARSE_ERROR(false,e.what(),filepath);
        return nullptr;
    }

    return style;
}

QVector<std::pair<json, json>> NoteStyleParser::buildNoteStageMap(
        const QVector<json>& stageJsons,
        const QVector<json>& songJsons,
        const QVector<QString>& stageFilePaths)
{
    QVector<std::pair<json, json>> result;

    // 构建舞台索引
    std::unordered_map<std::string, json> stageIndex;

    for (size_t i = 0; i < stageJsons.size(); i++) {
        // 从文件路径提取舞台 ID
        optional<QString> stageIdOpt = FileUtil::getPathLeaf(stageFilePaths[i]);
        if (!stageIdOpt.has_value()) continue;

        stageIndex[stageIdOpt.value().toStdString()] = stageJsons[i];
    }

    // 遍历歌曲
    for (const auto & songJson : songJsons) {
        // 解析歌曲数据
        json songData = songJson;
        if (songJson.contains("song") && songJson["song"].is_object()) {
            songData = songJson["song"];
        }

        // 获取歌曲指定的舞台
        std::string targetStage = "stage";
        if (songData.contains("stage") && songData["stage"].is_string())
        {
            targetStage = songData["stage"].get<std::string>();
        }
        // 没有的话，就直接使用默认的stage（就是默认的dadbattle的stage，已初始化为默认值）

        // 查找舞台
        auto it = stageIndex.find(targetStage);
        if (it != stageIndex.end()) {
            result.emplace_back(it->second, songJson);
        } else {
            // 找不到的话使用默认舞台
            auto defaultIt = stageIndex.find("stage");
            if (defaultIt != stageIndex.end()) {
                result.emplace_back(defaultIt->second, songJson);
            } else {
                // 没有stage字段，说明模组本身有问题，我们返回一个空的json对象，运行时解析的时候再使用默认值
                result.emplace_back(json::object(), songJson);
            }
        }
    }

    return result;
}
