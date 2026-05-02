//
// Created by kkplay on 4/22/26.
//

#pragma once
#include "utils/modding/ModScanUtils.h"
#include "IRegistrableEntry.h"
#include "string"
#include "utils/VersionUtil.h"

using std::string;
using std::unique_ptr;
using std::unordered_map;

// 更现代的C++，竟然有了泛型约束！
template<typename Y>
concept RegistryEntry = requires(Y t)
{
    requires std::is_default_constructible_v<Y>;
};
// 不过好像没什么用

/**
 * @brief: 模仿官方引擎的设计：所有关卡，歌曲，角色entry的基类，提供共有的方法
 * 每一个entry都可以看作一个单独的子系统，不同的类容纳的是上述枚举中列出来的对应类型
*/
template<typename T>
class BaseRegistry : public ISerializable
{
public:

    // 这个注册表的名字
    QString registryName;

    /**
     * {id, 实现了接口的内容}
     * 可以保证这些内容都是检查过的，可以直接使用，一定有效，不为空，顶多逻辑错误
    */
    unordered_map<QString,unique_ptr<ISerializable>> entries{};

    /**
     * 不初始化，直接野指针，出现各种难以调试的问题
    */
    static inline T* ins = nullptr;

    static T* instance()
    {
        if (!ins) ins = new T();
        return ins;
    }

    BaseRegistry(const QString& regName)
    {
        registryName = regName;
    }

    virtual void addEntry(const QString& id, unique_ptr<ISerializable> entry)
    {
        entries.insert({id,std::move(entry)});
    }

    // TODO: 这个还支持脚本化的entry，我这边由于没有解释器所以暂不支持。

    // 用于模组详情界面的“统计信息”，调用这个即可获得条目
    QString toString() const override
    {
        return registryName + "所持有的项数量: " + QString::number(entries.size());

    }

    QString oneToString(const QString& id) const override
    {
        auto it = entries.find(id);
        if (it == entries.end())
        {
            return "没有" + id + "这个项目！本注册表类型为" + registryName + "\n";
        }
        return it->second->toString();
    }

    // 禁止删除
protected:
    ~BaseRegistry() override = default;
};
