#pragma once

#include <QString>
#include <QVector>
#include <memory>
#include <optional>

#include "nlohmann/json.hpp"
#include "play/ISerializable.h"

using json = nlohmann::json;

/**
 * Psych Engine 角色文件结构
 *
 * 对应 PE Haxe 源码中的 CharacterFile 类型：
 * {
 *   animations: [{anim, name, fps, loop, indices, offsets}],
 *   image: String,
 *   scale: Float,
 *   sing_duration: Float,
 *   healthicon: String,
 *   position: [x, y],
 *   camera_position: [x, y],
 *   flip_x: Bool,
 *   no_antialiasing: Bool,
 *   healthbar_colors: [r, g, b],
 *   vocals_file: String
 * }
 *
 * 注意：这与 VS 的 CharacterData 完全独立，不做任何格式转换。
 */
struct PEAnimData : ISerializable
{
    QString anim;           // 动画名称 (e.g. "idle", "singLEFT")
    QString name;           // 帧前缀或符号名 (e.g. "BF idle dance", "BF note left 0")
    int fps = 24;           // 帧率
    bool loop = false;      // 是否循环
    QVector<int> indices;   // 帧索引（空=全部）
    QVector<int> offsets;   // 偏移 [x, y]

    void from_json(const json& j);
    QString toString() const override;
    QString oneToString(const QString& id) const override { return toString(); }
};

struct PECharacterData : ISerializable
{
    // 基础信息
    QString id;             // 角色 ID（文件名，不含扩展名）
    QString image;          // 图片路径（可能多个，逗号分隔）

    // ========== 视觉属性 ==========
    float scale = 1.0f;
    bool flipX = false;
    bool noAntialiasing = false;

    // ========== 游戏属性 ==========
    float singDuration = 4.0f;
    QString healthIcon;
    QString vocalsFile;

    // ========== 位置 ==========
    QVector<float> position = {0, 0};           // [x, y]
    QVector<float> cameraPosition = {0, 0};     // [x, y]

    // ========== 血条颜色 ==========
    QVector<int> healthbarColors = {161, 161, 161}; // [r, g, b]

    // ========== 动画 ==========
    QVector<PEAnimData> animations;

    void from_json(const json& j);
    json to_json() const;

    QString toString() const override;
    QString oneToString(const QString& id) const override { return toString(); }
};
