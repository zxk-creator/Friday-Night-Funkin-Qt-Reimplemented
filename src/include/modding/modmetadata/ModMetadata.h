//
// Created by kkplay on 4/16/26.
//

#pragma once
#include <QMap>
#include <vector>
#include <optional>

using std::optional;
using std::vector;
using std::string;

// 专门比较版本的
class VersionRules
{
public:
    string version;

    VersionRules() = default;

    explicit VersionRules(const string& version)
    {
        this->version = version;
    }

    VersionRules& operator=(const string& other)
    {
        version = other;
        return *this;
    }
};

using ModDependencies = QMap<string, VersionRules>;

// contributors数组里面的每一个元素
struct contributor
{
    string name;
    string roles;
    string url;
};

// 每个模组都是一个对象！
struct ModMetadata
{
    optional<string> id;
    optional<string> title;

    // 你应该通过调用getter方法获取我拼接好的字符串
    optional<string> description;
    // 想想还是拼接字符串算了，把这些人直接变成字符串方便qml显示
    optional<string> contributors;
    optional<string> homepage;
    optional<string> apiVersion;
    optional<string> modVersion;
    optional<string> license;
    optional<string> iconPath;
    optional<string> modPath;
    optional<ModDependencies> dependencies;
    optional<ModDependencies> optionalDependencies;

    // PE特有
    optional<bool> restart;
    optional<bool> runsGlobally;
    optional<std::vector<int>> bgRGBColor;

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
    QString toString() const
    {
        return getDetailString();
    }
    // 暂不支持序列化为JSON
};
