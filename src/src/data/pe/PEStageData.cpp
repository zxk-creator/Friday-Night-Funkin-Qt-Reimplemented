#include "data/pe/PEStageData.h"

void PEStageObject::from_json(const json& j)
{
    if (j.contains("type")) type = QString::fromStdString(j["type"].get<std::string>());
    if (j.contains("x")) x = j["x"].get<float>();
    if (j.contains("y")) y = j["y"].get<float>();
    if (j.contains("image")) image = QString::fromStdString(j["image"].get<std::string>());
    if (j.contains("scale")) scale = j["scale"].get<float>();
    if (j.contains("scroll") && j["scroll"].is_array() && j["scroll"].size() >= 2) {
        scroll.clear();
        scroll.push_back(j["scroll"][0].get<float>());
        scroll.push_back(j["scroll"][1].get<float>());
    }
    if (j.contains("color")) color = QString::fromStdString(j["color"].get<std::string>());
    if (j.contains("alpha")) alpha = j["alpha"].get<float>();
    if (j.contains("angle")) angle = j["angle"].get<float>();
    if (j.contains("antialiasing")) antialiasing = j["antialiasing"].get<bool>();
    if (j.contains("flipX")) flipX = j["flipX"].get<bool>();
    if (j.contains("flipY")) flipY = j["flipY"].get<bool>();
    if (j.contains("name")) name = QString::fromStdString(j["name"].get<std::string>());
    if (j.contains("firstAnimation")) firstAnimation = QString::fromStdString(j["firstAnimation"].get<std::string>());
}

void PEStageData::from_json(const json& j)
{
    if (j.contains("directory")) directory = QString::fromStdString(j["directory"].get<std::string>());
    if (j.contains("defaultZoom")) defaultZoom = j["defaultZoom"].get<float>();
    if (j.contains("isPixelStage")) isPixelStage = j["isPixelStage"].get<bool>();
    if (j.contains("stageUI")) stageUI = QString::fromStdString(j["stageUI"].get<std::string>());

    if (j.contains("boyfriend") && j["boyfriend"].is_array() && j["boyfriend"].size() >= 2) {
        boyfriend.clear();
        boyfriend.push_back(j["boyfriend"][0].get<float>());
        boyfriend.push_back(j["boyfriend"][1].get<float>());
    }
    if (j.contains("girlfriend") && j["girlfriend"].is_array() && j["girlfriend"].size() >= 2) {
        girlfriend.clear();
        girlfriend.push_back(j["girlfriend"][0].get<float>());
        girlfriend.push_back(j["girlfriend"][1].get<float>());
    }
    if (j.contains("opponent") && j["opponent"].is_array() && j["opponent"].size() >= 2) {
        opponent.clear();
        opponent.push_back(j["opponent"][0].get<float>());
        opponent.push_back(j["opponent"][1].get<float>());
    }

    if (j.contains("camera_boyfriend") && j["camera_boyfriend"].is_array() && j["camera_boyfriend"].size() >= 2) {
        cameraBoyfriend.clear();
        cameraBoyfriend.push_back(j["camera_boyfriend"][0].get<float>());
        cameraBoyfriend.push_back(j["camera_boyfriend"][1].get<float>());
    }
    if (j.contains("camera_opponent") && j["camera_opponent"].is_array() && j["camera_opponent"].size() >= 2) {
        cameraOpponent.clear();
        cameraOpponent.push_back(j["camera_opponent"][0].get<float>());
        cameraOpponent.push_back(j["camera_opponent"][1].get<float>());
    }
    if (j.contains("camera_girlfriend") && j["camera_girlfriend"].is_array() && j["camera_girlfriend"].size() >= 2) {
        cameraGirlfriend.clear();
        cameraGirlfriend.push_back(j["camera_girlfriend"][0].get<float>());
        cameraGirlfriend.push_back(j["camera_girlfriend"][1].get<float>());
    }

    if (j.contains("camera_speed")) cameraSpeed = j["camera_speed"].get<float>();
    if (j.contains("hide_girlfriend")) hideGirlfriend = j["hide_girlfriend"].get<bool>();

    if (j.contains("objects") && j["objects"].is_array()) {
        objects.clear();
        for (const auto& obj : j["objects"]) {
            PEStageObject stageObj;
            stageObj.from_json(obj);
            objects.push_back(stageObj);
        }
    }
}
