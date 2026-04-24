//
// Created by kkplay on 4/22/26.
//

#pragma once
#include "utils/modding/ModScanUtils.h"
#include "IRegistrableEntry.h"
#include "string"
#include "utils/VersionUtil.h"

using std::string;

enum class EntryType
{
    level,
    song,
    character,
    noteStyle
};

/**
 * @brief: 模仿官方引擎的设计：所有关卡，歌曲，角色entry的基类，提供共有的方法
 * 每一个entry都可以看作一个单独的子系统，不同的类容纳的是上述枚举中列出来的对应类型
*/
class BaseRegistry
{
public:

    BaseRegistry(EntryType type,const string& version)
    {
        this->thisEntryType = type;
        this->thisRequiredVersion = !VersionUtil::isValidSemVer(version) ? "1.0.x" : version;
    }

    EntryType thisEntryType = EntryType::level;

    string thisRequiredVersion;

    // TODO: 这个还支持脚本化的entry，我这边由于没有解释器所以暂不支持。
    // 先仅支持数据化配置的entry
    int countEntries()
    {
        return entries.size();
    }
};
