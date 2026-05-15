#include "data/stage/StageData.h"

#include "Constants.h"
#include "utils/exception/CustomException.h"
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

// ==================== StagePropData ====================
void StagePropData::from_json(const json& j) {
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

    if (j.contains("scale")) {
        const json& scaleJson = j["scale"];
        if (scaleJson.is_number()) {
            // 统一缩放：同时设置 X 和 Y
            scaleX = scaleJson.get<float>();
            scaleY = scaleJson.get<float>();
        } else if (scaleJson.is_array() && scaleJson.size() >= 2) {
            // 独立 XY 缩放：[scaleX, scaleY]
            scaleX = scaleJson[0].get<float>();
            scaleY = scaleJson[1].get<float>();
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

QString StagePropData::toString() const
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

    res += "  物品缩放X: " + QString::number(scaleX) + "物品缩放Y: " + QString::number(scaleY) + "\n";

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
            StagePropData prop;
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

unique_ptr<StageData> StageDataParser::parseStageData_VS(const json& j, const QString& filepath) {
    try {
        auto data = make_unique<StageData>();
        data->from_json(j);

        // 验证必填字段
        if (data->name.isEmpty()) {
            LOG_WARNING(false,"错误: StageData缺少Name字段" + filepath);
            return nullptr;
        }

        return data;
    } catch (const std::exception& e) {
        LOG_WARNING(false,"从" + filepath + "中解析stagedata失败，原因: " + e.what());
        return nullptr;
    }
}

unique_ptr<StageData> StageDataParser::parseStageData_PE(const json& j, const QString& filename)
{
    auto stage = std::make_unique<StageData>();

    try {
        // ========== 1. 基础信息 ==========
        stage->version = StageDataRelative::STAGE_DATA_VERSION;

        if (j.contains("directory")) {
            stage->directory = QString::fromStdString(j["directory"].get<std::string>());
        } else {
            stage->directory = "shared";
        }

        // 名称（从文件名提取）
        stage->name = QFileInfo(filename).baseName();

        // ========== 2. 相机设置 ==========
        if (j.contains("defaultZoom")) {
            stage->cameraZoom = j["defaultZoom"].get<float>();
        } else {
            stage->cameraZoom = 1.0f;
        }

        if (j.contains("camera_speed") && !j["camera_speed"].is_null()) {
            stage->cameraSpeed = j["camera_speed"].get<float>();
        } else {
            stage->cameraSpeed = 1.0f;
        }

        // ========== 3. 角色位置 ==========
        // Boyfriend
        if (j.contains("boyfriend") && j["boyfriend"].is_array() && j["boyfriend"].size() >= 2) {
            stage->characters.bf.position.clear();
            stage->characters.bf.position.push_back(j["boyfriend"][0].get<float>());
            stage->characters.bf.position.push_back(j["boyfriend"][1].get<float>());
        }

        // Girlfriend
        if (j.contains("girlfriend") && j["girlfriend"].is_array() && j["girlfriend"].size() >= 2) {
            stage->characters.gf.position.clear();
            stage->characters.gf.position.push_back(j["girlfriend"][0].get<float>());
            stage->characters.gf.position.push_back(j["girlfriend"][1].get<float>());
        }

        // Opponent (Dad)
        if (j.contains("opponent") && j["opponent"].is_array() && j["opponent"].size() >= 2) {
            stage->characters.dad.position.clear();
            stage->characters.dad.position.push_back(j["opponent"][0].get<float>());
            stage->characters.dad.position.push_back(j["opponent"][1].get<float>());
        }

        // ========== 4. 相机偏移 ==========
        if (j.contains("camera_boyfriend") && j["camera_boyfriend"].is_array() && j["camera_boyfriend"].size() >= 2) {
            stage->characters.bf.cameraOffsets.clear();
            stage->characters.bf.cameraOffsets.push_back(j["camera_boyfriend"][0].get<float>());
            stage->characters.bf.cameraOffsets.push_back(j["camera_boyfriend"][1].get<float>());
        } else {
            stage->characters.bf.cameraOffsets = {-100, -100};
        }

        if (j.contains("camera_opponent") && j["camera_opponent"].is_array() && j["camera_opponent"].size() >= 2) {
            stage->characters.dad.cameraOffsets.clear();
            stage->characters.dad.cameraOffsets.push_back(j["camera_opponent"][0].get<float>());
            stage->characters.dad.cameraOffsets.push_back(j["camera_opponent"][1].get<float>());
        } else {
            stage->characters.dad.cameraOffsets = {100, -100};
        }

        if (j.contains("camera_girlfriend") && j["camera_girlfriend"].is_array() && j["camera_girlfriend"].size() >= 2) {
            stage->characters.gf.cameraOffsets.clear();
            stage->characters.gf.cameraOffsets.push_back(j["camera_girlfriend"][0].get<float>());
            stage->characters.gf.cameraOffsets.push_back(j["camera_girlfriend"][1].get<float>());
        } else {
            stage->characters.gf.cameraOffsets = {0, 0};
        }

        // ========== 5. 滚动速度（默认值） ==========
        // 这个主要用于视差效果，近的跑得快，远的跑的慢效果。
        stage->characters.bf.scroll = {1, 1};
        stage->characters.dad.scroll = {1, 1};
        stage->characters.gf.scroll = {1, 1};

        // ========== 6. 道具列表 ==========
        // 事实上，这个我看很少有人用，都比较喜欢直接使用lua创建。（可能是PE最新特性支持有限？）
        if (j.contains("objects") && j["objects"].is_array()) {
            for (const auto& objJson : j["objects"]) {
                StagePropData prop;

                if (objJson.contains("name")) {
                    prop.name = QString::fromStdString(objJson["name"].get<std::string>());
                }

                // 特殊处理内置角色
                if (objJson.contains("type")) {
                    QString type = QString::fromStdString(objJson["type"].get<std::string>());
                    if (type == "gf" || type == "gfGroup") {
                        continue;
                    }
                    if (type == "dad" || type == "dadGroup") {
                        continue;
                    }
                    if (type == "boyfriend" || type == "boyfriendGroup") {
                        continue;
                    }
                }

                // 资源路径
                if (objJson.contains("image")) {
                    prop.assetPath = QString::fromStdString(objJson["image"].get<std::string>());
                } else {
                    prop.assetPath = "shared:" + QString::fromStdString(objJson["name"].get<std::string>());
                }

                // 位置
                if (objJson.contains("x") && objJson.contains("y")) {
                    prop.position = {objJson["x"].get<float>(), objJson["y"].get<float>()};
                } else if (objJson.contains("position") && objJson["position"].is_array() && objJson["position"].size() >= 2) {
                    prop.position = {objJson["position"][0].get<float>(), objJson["position"][1].get<float>()};
                } else {
                    prop.position = {0, 0};
                }

                // Z 轴顺序
                if (objJson.contains("zIndex")) {
                    prop.zIndex = objJson["zIndex"].get<int>();
                } else {
                    prop.zIndex = 0;
                }

                // 缩放
                if (objJson.contains("scale") && objJson["scale"].is_array() && objJson["scale"].size() >= 2) {
                    // 官方使用 Either，这里用数组
                    prop.scaleX = objJson["scale"][0].get<float>();
                    prop.scaleY = objJson["scale"][1].get<float>();
                } else if (objJson.contains("scale") && objJson["scale"].is_number()) {
                    float s = objJson["scale"].get<float>();
                    prop.scaleX = s;
                    prop.scaleY = s;
                }

                // 滚动因子
                if (objJson.contains("scroll") && objJson["scroll"].is_array() && objJson["scroll"].size() >= 2) {
                    prop.scroll = {objJson["scroll"][0].get<float>(), objJson["scroll"][1].get<float>()};
                } else {
                    prop.scroll = {1, 1};
                }

                // 透明度
                if (objJson.contains("alpha")) {
                    prop.alpha = objJson["alpha"].get<float>();
                } else {
                    prop.alpha = 1.0f;
                }

                // 旋转角度
                if (objJson.contains("angle")) {
                    prop.angle = objJson["angle"].get<float>();
                }

                // 颜色
                if (objJson.contains("color")) {
                    prop.color = QString::fromStdString(objJson["color"].get<std::string>());
                }

                // 混合模式
                if (objJson.contains("blend")) {
                    prop.blend = QString::fromStdString(objJson["blend"].get<std::string>());
                }

                // 是否为像素风格
                if (j.contains("isPixelStage") && j["isPixelStage"].is_boolean()) {
                    prop.isPixel = j["isPixelStage"].get<bool>();
                }

                // 动画（似乎好像可能只有最新版才支持这样）
                if (objJson.contains("animations") && objJson["animations"].is_array()) {
                    for (const auto& animJson : objJson["animations"]) {
                        AnimationData anim;

                        if (animJson.contains("anim")) {
                            anim.name = QString::fromStdString(animJson["anim"].get<std::string>());
                        }
                        if (animJson.contains("name")) {
                            anim.prefix = QString::fromStdString(animJson["name"].get<std::string>());
                        }
                        if (animJson.contains("fps")) {
                            anim.frameRate = animJson["fps"].get<int>();
                        }
                        if (animJson.contains("loop")) {
                            anim.looped = animJson["loop"].get<bool>();
                        }
                        if (animJson.contains("offsets") && animJson["offsets"].is_array() && animJson["offsets"].size() >= 2) {
                            anim.offsets = {animJson["offsets"][0].get<float>(), animJson["offsets"][1].get<float>()};
                        }
                        if (animJson.contains("indices") && animJson["indices"].is_array()) {
                            for (const auto& idx : animJson["indices"]) {
                                anim.frameIndices.push_back(idx.get<int>());
                            }
                        }

                        prop.animations.push_back(anim);
                    }

                    // 起始动画
                    if (objJson.contains("firstAnimation")) {
                        prop.startingAnimation = QString::fromStdString(objJson["firstAnimation"].get<std::string>());
                    }
                }

                stage->props.push_back(prop);
            }
        }

        if (j.contains("preload")) {
            // 官方结构中没有preload，忽略
        }

    } catch (const std::exception& e) {
        LOG_JSON_PARSE_ERROR(false,e.what(),filename);
    }

    return stage;
}