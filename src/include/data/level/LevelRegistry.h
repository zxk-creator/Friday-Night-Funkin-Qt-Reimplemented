//
// Created by kkplay on 4/22/26.
//

#pragma once

#include "data/BaseRegistry.h"
#include "data/IRegistrableEntry.h"

using Super = BaseRegistry;

class LevelRegistry : public IRegistrableEntry, public BaseRegistry
{
public:
    static inline const string LEVEL_DATA_VERSION = "1.0.1";

    /**
     * 每一个键值对都是一个该类型的对象
     */
    std::unordered_map<EntryType,LevelData> entries;

    LevelRegistry() : Super(EntryType::level, LEVEL_DATA_VERSION)
    {

    }

    void loadEntries() override
    {
        
    }
};
