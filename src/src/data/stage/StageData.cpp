#include "data/stage/StageData.h"

#include "utils/message/MessageHandler.h"

using namespace std;

void StageDataCharacter::from_json(const json& j) {
    if (j.contains("zIndex")) zIndex = j["zIndex"].get<int>();
    if (j.contains("position") && j["position"].is_array()) {
        position.clear();
        for (const auto& val : j["position"]) {
            position.push_back(val.get<float>());
        }
    }
    if (j.contains("scale")) scale = j["scale"].get<float>();
    if (j.contains("cameraOffsets") && j["cameraOffsets"].is_array()) {
        cameraOffsets.clear();
        for (const auto& val : j["cameraOffsets"]) {
            cameraOffsets.push_back(val.get<float>());
        }
    }
    if (j.contains("scroll") && j["scroll"].is_array()) {
        scroll.clear();
        for (const auto& val : j["scroll"]) {
            scroll.push_back(val.get<float>());
        }
    }
    if (j.contains("alpha")) alpha = j["alpha"].get<float>();
    if (j.contains("angle")) angle = j["angle"].get<float>();
}

QString StageDataCharacter::toString() const
{
    QString res = "角色数据:\n";
    res += "  层级索引: " + QString::number(zIndex) + "\n";

    res += "  位置: [";
    for (int i = 0; i < position.size(); ++i) {
        res += QString::number(position[i]);
        if (i < position.size() - 1) res += ", ";
    }
    res += "]\n";

    res += "  缩放: " + QString::number(scale) + "\n";

    if (!cameraOffsets.isEmpty()) {
        res += "  相机偏移: [";
        for (int i = 0; i < cameraOffsets.size(); ++i) {
            res += QString::number(cameraOffsets[i]);
            if (i < cameraOffsets.size() - 1) res += ", ";
        }
        res += "]\n";
    }

    res += "  滚动系数: [";
    for (int i = 0; i < scroll.size(); ++i) {
        res += QString::number(scroll[i]);
        if (i < scroll.size() - 1) res += ", ";
    }
    res += "]\n";

    res += "  透明度: " + QString::number(alpha) + "\n";
    res += "  角度: " + QString::number(angle) + "度";

    return res;
}

// ==================== StageDataCharacters ====================
void StageDataCharacters::from_json(const json& j) {
    if (j.contains("bf")) bf.from_json(j["bf"]);
    if (j.contains("dad")) dad.from_json(j["dad"]);
    if (j.contains("gf")) gf.from_json(j["gf"]);
}

QString StageDataCharacters::toString() const
{
    QString res = "角色组数据:\n";
    res += "----- 男友角色(BF) -----\n" + bf.toString() + "\n";
    res += "----- 老爹角色(老爹) -----\n" + dad.toString() + "\n";
    res += "----- 女友角色(GF) -----\n" + gf.toString();
    return res;
}

// ==================== StageDataProp ====================
void StageDataProp::from_json(const json& j) {
    if (j.contains("name")) name = QString::fromStdString(j["name"].get<std::string>());
    if (j.contains("assetPath")) assetPath = QString::fromStdString(j["assetPath"].get<std::string>());

    if (j.contains("position") && j["position"].is_array()) {
        position.clear();
        for (const auto& val : j["position"]) {
            position.push_back(val.get<float>());
        }
    }

    if (j.contains("zIndex")) zIndex = j["zIndex"].get<int>();
    if (j.contains("isPixel")) isPixel = j["isPixel"].get<bool>();
    if (j.contains("flipX")) flipX = j["flipX"].get<bool>();
    if (j.contains("flipY")) flipY = j["flipY"].get<bool>();

    // scale: Either<Float, Array<Float>>
    if (j.contains("scale")) {
        const json& scaleJson = j["scale"];
        if (scaleJson.is_number()) {
            scale = scaleJson.get<float>();
        } else if (scaleJson.is_array()) {
            QVector<float> arr;
            for (const auto& val : scaleJson) {
                arr.push_back(val.get<float>());
            }
            scale = arr;
        }
    }

    if (j.contains("alpha")) alpha = j["alpha"].get<float>();
    if (j.contains("danceEvery")) danceEvery = j["danceEvery"].get<float>();

    if (j.contains("scroll") && j["scroll"].is_array()) {
        scroll.clear();
        for (const auto& val : j["scroll"]) {
            scroll.push_back(val.get<float>());
        }
    }

    if (j.contains("animations") && j["animations"].is_array()) {
        animations.clear();
        for (const auto& animJson : j["animations"]) {
            AnimationData anim;
            anim.from_json(animJson);
            animations.push_back(anim);
        }
    }

    if (j.contains("startingAnimation")) startingAnimation = QString::fromStdString(j["startingAnimation"].get<std::string>());
    if (j.contains("animType")) animType = QString::fromStdString(j["animType"].get<std::string>());
    if (j.contains("angle")) angle = j["angle"].get<float>();
    if (j.contains("blend")) blend = QString::fromStdString(j["blend"].get<std::string>());
    if (j.contains("color")) color = QString::fromStdString(j["color"].get<std::string>());

    if (j.contains("atlasSettings")) {
        atlasSettings = TextureAtlasData();
        atlasSettings->from_json(j["atlasSettings"]);
    }
}

QString StageDataProp::toString() const
{
    QString res = "道具数据:\n";

    if (name.has_value()) {
        res += "  名称: " + name.value() + "\n";
    }

    res += "  资源路径: " + assetPath + "\n";

    res += "  位置: [";
    for (int i = 0; i < position.size(); ++i) {
        res += QString::number(position[i]);
        if (i < position.size() - 1) res += ", ";
    }
    res += "]\n";

    res += "  层级索引: " + QString::number(zIndex) + "\n";
    res += "  是否像素风格: " + QString(isPixel ? "是" : "否") + "\n";
    res += "  水平翻转: " + QString(flipX ? "是" : "否") + "\n";
    res += "  垂直翻转: " + QString(flipY ? "是" : "否") + "\n";

    res += "  缩放: ";
    if (std::holds_alternative<float>(scale)) {
        res += QString::number(std::get<float>(scale)) + " (统一缩放)\n";
    } else {
        const auto& vec = std::get<QVector<float>>(scale);
        res += "[";
        for (int i = 0; i < vec.size(); ++i) {
            res += QString::number(vec[i]);
            if (i < vec.size() - 1) res += ", ";
        }
        res += "] (X,Y独立缩放)\n";
    }

    res += "  透明度: " + QString::number(alpha) + "\n";
    res += "  跳舞间隔: " + QString::number(danceEvery) + "\n";

    res += "  滚动系数: [";
    for (int i = 0; i < scroll.size(); ++i) {
        res += QString::number(scroll[i]);
        if (i < scroll.size() - 1) res += ", ";
    }
    res += "]\n";

    res += "  动画数量: " + QString::number(animations.size()) + "\n";

    if (startingAnimation.has_value()) {
        res += "  起始动画: " + startingAnimation.value() + "\n";
    }

    res += "  动画类型: " + animType + "\n";
    res += "  角度: " + QString::number(angle) + "度\n";

    if (!blend.isEmpty()) {
        res += "  混合模式: " + blend + "\n";
    }

    res += "  颜色: " + color + "\n";

    if (atlasSettings.has_value()) {
        res += "  纹理图集设置: 已配置\n";
    }

    return res;
}

// ==================== StageData ====================
void StageData::from_json(const json& j) {
    if (j.contains("version")) version = QString::fromStdString(j["version"].get<std::string>());
    if (j.contains("name")) name = QString::fromStdString(j["name"].get<std::string>());

    if (j.contains("props") && j["props"].is_array()) {
        props.clear();
        for (const auto& propJson : j["props"]) {
            StageDataProp prop;
            prop.from_json(propJson);
            props.push_back(prop);
        }
    }

    if (j.contains("characters")) characters.from_json(j["characters"]);

    if (j.contains("cameraZoom")) cameraZoom = j["cameraZoom"].get<float>();
    if (j.contains("directory")) directory = QString::fromStdString(j["directory"].get<std::string>());
}

QString StageData::toString() const
{
    QString res = "舞台数据:\n";
    res += "  版本: " + version + "\n";
    res += "  名称: " + name + "\n";

    if (cameraZoom.has_value()) {
        res += "  相机缩放: " + QString::number(cameraZoom.value()) + "\n";
    }

    if (directory.has_value()) {
        res += "  资源目录: " + directory.value() + "\n";
    }

    res += "\n================== 道具列表 ==================\n";
    if (props.isEmpty()) {
        res += "  无道具\n";
    } else {
        for (int i = 0; i < props.size(); ++i) {
            res += "\n道具 #" + QString::number(i + 1) + ":\n";
            res += props[i].toString();
            if (i < props.size() - 1) res += "\n";
        }
    }

    res += "\n================== 角色数据 ==================\n";
    res += characters.toString();

    return res;
}

unique_ptr<StageData> StageDataParser::parseStageData(const json& j, const QString& filepath) {
    try {
        auto data = make_unique<StageData>();
        data->from_json(j);

        // 验证必填字段
        if (data->name.isEmpty()) {
            LOG_WARNING("错误: StageData缺少Name字段" + filepath);
            return nullptr;
        }

        return data;
    } catch (const std::exception& e) {
        LOG_WARNING("从" + filepath + "中解析stagedata失败，原因: " + e.what());
        return nullptr;
    }
}