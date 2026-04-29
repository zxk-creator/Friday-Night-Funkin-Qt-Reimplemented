//
// Created by kkplay on 4/22/26.
//

#pragma once

#include "LevelData.h"
#include "data/BaseRegistry.h"
#include "data/IRegistrableEntry.h"

using Super = BaseRegistry;

class LevelRegistry : public IRegistrableEntry, public BaseRegistry
{
public:
    static inline const string LEVEL_DATA_VERSION = "1.0.1";

    LevelRegistry()
    {

    }

    void loadEntries() override
    {
        
    }

    /**
     * 添加可用周到注册表中！
     * @param levelData 周配置数据
     * @param fileName 调试用：文件名防止版本不对导致错误
     */
    static void addAvailableWeeks(LevelData& levelData,QString fileName)
    {
        // 版本不匹配就直接跳过这个模组了
        if (!VersionUtil::validateVersionStr(levelData.version,LEVEL_DATA_VERSION))
        {
            Exception::logVersionInvalid(levelData.version,LEVEL_DATA_VERSION,fileName.toStdString());
            return;
        }
        // 运行到这里就没问题了
        entries.insert({levelData.name, levelData});
    }

    static optional<LevelData> getLevelData(const string& levelName)
    {
        if (entries.find(levelName) != entries.end())
            return entries[levelName];

        return nullopt;
    }

private:
    /**
     * 每一个键值对都是一个该类型的对象
     * 此条目也适用于storymenu内容的显示
     * @param string: 周名称(ID)。LevelData，实际存储的周配置数据
     */
    static std::unordered_map<string,LevelData> entries;
};
