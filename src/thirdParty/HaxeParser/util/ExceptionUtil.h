//
// Created by kkplay on 7/7/26.
//

#pragma once
#include <stdexcept>
#include <QString>

namespace HaxeError {
    enum class OperatorKind
    {
        add,
        subtract,
        multiply,
        divide,
        compare
    };

    inline void throwUnsupportedOperator(OperatorKind op)
    {
        QString opName;
        switch (op) {
        case OperatorKind::add:
            opName = "加法";
            break;
        case OperatorKind::subtract:
            opName = "减法";
            break;
        case OperatorKind::multiply:
            opName = "乘法";
            break;
        case OperatorKind::divide:
            opName = "除法";
            break;
        case OperatorKind::compare:
            opName = "比较";
            break;
        default:
            opName = "未知运算符";
            break;
        }
        throw std::runtime_error(QString("不支持的运算符操作: " + opName).toStdString());
    }

    inline void throwDivideZero()
    {
        throw std::runtime_error("除数不能为零！");
    }

    inline void throwBadKind(const QString& kind)
    {
        throw std::runtime_error(QString("不是类型: " + kind).toStdString());
    }

    inline void throwLogicalTypeError(const QString& operation)
    {
        throw std::runtime_error(QString("逻辑运算错误: " + operation + " 只能用于布尔值").toStdString());
    }

    inline void throwComparisonTypeError(const QString& operation)
    {
        throw std::runtime_error(QString("比较运算错误: " + operation + " 不支持当前类型").toStdString());
    }

    inline void throwConversionError(const QString& from, const QString& to)
    {
        throw std::runtime_error(QString("类型转换错误: 无法从 " + from + " 转换为 " + to).toStdString());
    }

    inline void throwRuntimeError(const QString& msg)
    {
        throw std::runtime_error(msg.toStdString());
    }
}