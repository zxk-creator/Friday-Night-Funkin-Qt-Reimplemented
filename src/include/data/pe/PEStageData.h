#pragma once

#include <QString>
#include <QVector>
#include <QVariant>
#include <optional>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

/**
 * Psych Engine 舞台数据结构（独立于 VS）
 *
 * 对应 PE StageFile 格式：
 * {
 *   "directory": "week1",
 *   "defaultZoom": 0.9,
 *   "isPixelStage": false,
 *   "stageUI": "normal",
 *   "boyfriend": [x, y],
 *   "girlfriend": [x, y],
 *   "opponent": [x, y],
 *   "camera_boyfriend": [x, y],
 *   "camera_opponent": [x, y],
 *   "camera_girlfriend": [x, y],
 *   "camera_speed": 1.0,
 *   "hide_girlfriend": false,
 *   "objects": [...]
 * }
 */

struct PEStageObject
{
    QString type;                       // "square", "sprite", "animatedSprite"
    float x = 0.0f;
    float y = 0.0f;
    QString image;
    float scale = 1.0f;
    QVector<float> scroll = {1.0f, 1.0f};
    QString color = "#FFFFFF";
    float alpha = 1.0f;
    float angle = 0.0f;
    bool antialiasing = true;
    bool flipX = false;
    bool flipY = false;
    QString name;
    QString firstAnimation;

    void from_json(const json& j);
};

struct PEStageData
{
    QString directory;
    float defaultZoom = 0.9f;
    bool isPixelStage = false;
    QString stageUI = "normal";

    // 角色位置
    QVector<float> boyfriend = {0, 0};
    QVector<float> girlfriend = {0, 0};
    QVector<float> opponent = {0, 0};

    // 相机偏移
    QVector<float> cameraBoyfriend = {0, 0};
    QVector<float> cameraOpponent = {0, 0};
    QVector<float> cameraGirlfriend = {0, 0};
    float cameraSpeed = 1.0f;

    bool hideGirlfriend = false;

    // 舞台对象
    QVector<PEStageObject> objects;

    void from_json(const json& j);
};
