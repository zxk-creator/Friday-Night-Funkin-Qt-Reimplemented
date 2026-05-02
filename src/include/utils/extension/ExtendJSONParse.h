//
// Created by kkplay on 5/1/26.
//

#pragma once

#include "nlohmann/json.hpp"
#include <QString>

// 拓展nlohmann解析库，使其能够直接获得QString，不用手动转换！
// 不过好像并没有实现
namespace nlohmann {
    template <>
    struct adl_serializer<QString> {
        static void to_json(json& j, const QString& str) {
            j = str.toStdString();
        }

        static void from_json(const json& j, QString& str) {
            str = QString::fromStdString(j.get<std::string>());
        }
    };
}