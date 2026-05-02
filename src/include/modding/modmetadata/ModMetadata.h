//
// Created by kkplay on 4/16/26.
//

#pragma once
#include <QMap>
#include <vector>
#include <optional>
#include "modding/ModEngineType.h"
#include "play/ISerializable.h"

using std::optional;
using std::vector;

using ModDependencies = QMap<QString, QString>;

// contributors数组里面的每一个元素
struct contributor
{
    QString name;
    QString roles;
    QString url;
};

// 每个模组都是一个对象！
struct ModMetadata : ISerializable
{
    ModEngineType engineType = ModEngineType::PE;

    optional<QString> id;
    optional<QString> title;

    // 你应该通过调用getter方法获取我拼接好的字符串
    optional<QString> description;
    // 想想还是拼接字符串算了，把这些人直接变成字符串方便qml显示
    optional<QString> contributors;
    optional<QString> homepage;
    optional<QString> apiVersion;
    optional<QString> modVersion;
    optional<QString> license;
    optional<QString> iconPath;
    // 这东西必须存在，不然还解析什么
    QString modPath;
    optional<ModDependencies> dependencies;
    optional<ModDependencies> optionalDependencies;

    // PE特有
    bool restart = false;
    bool runsGlobally = false;
    std::vector<int> bgRGBColor = {255,228,0};

    ModMetadata();

private:
    // 返回descriptiom，contributor，homepage拼接成的字符串
    QString getMainInfoString() const;

    // 返回license, dependencies, optionalDependencies拼接成的字符串
    QString getExtraInfoString() const;

public:
    QString getDetailString() const
    {
        return getMainInfoString() + getExtraInfoString();
    }

    // 打印他的内容
    QString toString() const override
    {
        return getDetailString();
    }

    // 不应该调用这个
    QString oneToString(const QString& id) const override
    {
        return "";
    }
};
