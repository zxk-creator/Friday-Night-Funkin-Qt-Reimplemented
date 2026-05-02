//
// Created by kkplay on 5/1/26.
//

#pragma once
#include <QString>

// "接口" 用于提供各种序列化函数
class ISerializable
{
public:

    // 整个内容序列化为字符串，方便QML显示
    [[nodiscard]] virtual QString toString() const = 0;

    // 单个内容序列化为字符串，适合Registry
    [[nodiscard]] virtual QString oneToString(const QString& id) const = 0;

    virtual ~ISerializable() = default;
};
