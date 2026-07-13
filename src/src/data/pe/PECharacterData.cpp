#include "data/pe/PECharacterData.h"
#include "utils/message/MessageHandler.h"

void PEAnimData::from_json(const json& j)
{
    if (j.contains("anim")) anim = QString::fromStdString(j["anim"].get<std::string>());
    if (j.contains("name")) name = QString::fromStdString(j["name"].get<std::string>());
    if (j.contains("fps")) fps = j["fps"].get<int>();
    if (j.contains("loop")) loop = j["loop"].get<bool>();
    if (j.contains("indices") && j["indices"].is_array()) {
        indices.clear();
        for (const auto& idx : j["indices"])
            indices.push_back(idx.get<int>());
    }
    if (j.contains("offsets") && j["offsets"].is_array() && j["offsets"].size() >= 2) {
        offsets.clear();
        offsets.push_back(j["offsets"][0].get<int>());
        offsets.push_back(j["offsets"][1].get<int>());
    }
}

QString PEAnimData::toString() const
{
    return QString("  动画: %1 | 前缀: %2 | FPS: %3 | 循环: %4 | 偏移: [%5, %6]")
        .arg(anim, name)
        .arg(fps)
        .arg(loop ? "是" : "否")
        .arg(offsets.size() > 0 ? offsets[0] : 0)
        .arg(offsets.size() > 1 ? offsets[1] : 0);
}

void PECharacterData::from_json(const json& j)
{
    // 基础信息
    if (j.contains("image")) image = QString::fromStdString(j["image"].get<std::string>());

    // 视觉属性
    if (j.contains("scale")) scale = j["scale"].get<float>();
    if (j.contains("flip_x")) flipX = j["flip_x"].get<bool>();
    if (j.contains("no_antialiasing")) noAntialiasing = j["no_antialiasing"].get<bool>();

    // 游戏属性
    if (j.contains("sing_duration")) singDuration = j["sing_duration"].get<float>();
    if (j.contains("healthicon")) healthIcon = QString::fromStdString(j["healthicon"].get<std::string>());
    if (j.contains("vocals_file")) vocalsFile = QString::fromStdString(j["vocals_file"].get<std::string>());

    // 位置
    if (j.contains("position") && j["position"].is_array() && j["position"].size() >= 2) {
        position.clear();
        position.push_back(j["position"][0].get<float>());
        position.push_back(j["position"][1].get<float>());
    }
    if (j.contains("camera_position") && j["camera_position"].is_array() && j["camera_position"].size() >= 2) {
        cameraPosition.clear();
        cameraPosition.push_back(j["camera_position"][0].get<float>());
        cameraPosition.push_back(j["camera_position"][1].get<float>());
    }

    // 血条颜色
    if (j.contains("healthbar_colors") && j["healthbar_colors"].is_array() && j["healthbar_colors"].size() >= 3) {
        healthbarColors.clear();
        healthbarColors.push_back(j["healthbar_colors"][0].get<int>());
        healthbarColors.push_back(j["healthbar_colors"][1].get<int>());
        healthbarColors.push_back(j["healthbar_colors"][2].get<int>());
    }

    // 动画
    if (j.contains("animations") && j["animations"].is_array()) {
        animations.clear();
        for (const auto& animJson : j["animations"]) {
            PEAnimData anim;
            anim.from_json(animJson);
            animations.push_back(anim);
        }
    }
}

json PECharacterData::to_json() const
{
    json j;
    j["image"] = image.toStdString();
    j["scale"] = scale;
    j["flip_x"] = flipX;
    j["no_antialiasing"] = noAntialiasing;
    j["sing_duration"] = singDuration;
    j["healthicon"] = healthIcon.toStdString();
    if (!vocalsFile.isEmpty()) j["vocals_file"] = vocalsFile.toStdString();
    j["position"] = { position.value(0), position.value(1) };
    j["camera_position"] = { cameraPosition.value(0), cameraPosition.value(1) };
    j["healthbar_colors"] = { healthbarColors.value(0), healthbarColors.value(1), healthbarColors.value(2) };

    json anims = json::array();
    for (const auto& anim : animations) {
        json a;
        a["anim"] = anim.anim.toStdString();
        a["name"] = anim.name.toStdString();
        a["fps"] = anim.fps;
        a["loop"] = anim.loop;
        a["indices"] = json::array();
        for (int idx : anim.indices) a["indices"].push_back(idx);
        a["offsets"] = { anim.offsets.value(0, 0), anim.offsets.value(1, 0) };
        anims.push_back(a);
    }
    j["animations"] = anims;
    return j;
}

QString PECharacterData::toString() const
{
    QString res;
    res += "========== PE 角色数据 ==========\n";
    res += "ID: " + id + "\n";
    res += "图片: " + image + "\n";
    res += "缩放: " + QString::number(scale) + "\n";
    res += "翻转: " + QString(flipX ? "是" : "否") + "\n";
    res += "抗锯齿: " + QString(noAntialiasing ? "禁用" : "启用") + "\n";
    res += "唱歌时长: " + QString::number(singDuration) + "\n";
    res += "健康图标: " + healthIcon + "\n";
    res += "人声文件: " + vocalsFile + "\n";
    res += "位置: [" + QString::number(position.value(0)) + ", " + QString::number(position.value(1)) + "]\n";
    res += "相机位置: [" + QString::number(cameraPosition.value(0)) + ", " + QString::number(cameraPosition.value(1)) + "]\n";
    res += "血条颜色: [" + QString::number(healthbarColors.value(0)) + ", "
           + QString::number(healthbarColors.value(1)) + ", " + QString::number(healthbarColors.value(2)) + "]\n";
    res += "动画数量: " + QString::number(animations.size()) + "\n";
    for (const auto& anim : animations)
        res += "  " + anim.toString() + "\n";
    return res;
}
