//
// Created by kkplay on 4/22/26.
//

#pragma once

#include "nlohmann/json.hpp"
#include "play/ISerializable.h"

using json = nlohmann::json;

struct TextureAtlasData : ISerializable {
    bool swfMode = false;
    bool cacheOnLoad = false;
    int filterQuality = 1;      // 0=HIGH,1=MEDIUM,2=LOW,3=RUDY
    bool applyStageMatrix = false;
    bool useRenderTexture = false;

    void from_json(const json& j) {
        if (j.contains("swfMode")) swfMode = j["swfMode"].get<bool>();
        if (j.contains("cacheOnLoad")) cacheOnLoad = j["cacheOnLoad"].get<bool>();
        if (j.contains("filterQuality")) filterQuality = j["filterQuality"].get<int>();
        if (j.contains("applyStageMatrix")) applyStageMatrix = j["applyStageMatrix"].get<bool>();
        if (j.contains("useRenderTexture")) useRenderTexture = j["useRenderTexture"].get<bool>();
    }

    QString toString() const override
    {
        QString res;
        res += "SWF模式: " + QString(swfMode ? "是" : "否") + "\n";
        res += "加载时缓存: " + QString(cacheOnLoad ? "是" : "否") + "\n";

        QString qualityStr;
        switch (filterQuality) {
        case 0: qualityStr = "高"; break;
        case 1: qualityStr = "中"; break;
        case 2: qualityStr = "低"; break;
        case 3: qualityStr = "极低"; break;
        default: qualityStr = "未知"; break;
        }
        res += "过滤质量: " + qualityStr + " (" + QString::number(filterQuality) + ")\n";
        res += "应用舞台矩阵: " + QString(applyStageMatrix ? "是" : "否") + "\n";
        res += "使用渲染纹理: " + QString(useRenderTexture ? "是" : "否") + "\n";

        return res;
    }

    QString oneToString(const QString& id) const override
    {
        return toString();
    }
};