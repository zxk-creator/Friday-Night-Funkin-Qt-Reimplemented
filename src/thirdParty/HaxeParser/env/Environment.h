
#pragma once
#include <QMap>
#include <QString>
#include <memory>
#include "../token/Token.h"

class Dynamic;

// 存储脚本变量用的类
class Environment {
    // 存储当前作用域中的所有变量名: 值，如{x: 5, y: "Hello World"}
    QMap<QString, Dynamic> values;
    // 外层环境指针，指向其外层作用域(一个{})。内层作用域可以访问外层作用域变量，但是外层访问不到内层的。内层找不到就去外层找
    std::shared_ptr<Environment> enclosing;

public:
    // 全局环境构造
    Environment() : enclosing(nullptr) {}

    // 嵌套环境构造
    explicit Environment(std::shared_ptr<Environment> enclosing)
        : enclosing(enclosing) {}

    // 在当前环境定义新变量
    void define(const QString& name, const Dynamic& value) {
        values[name] = value;
    }

    // 从内向外查找变量
    Dynamic get(const Token& name) {
        if (values.contains(name.lexeme)) {
            return values[name.lexeme];
        }

        // 去外层环境找
        if (enclosing) {
            return enclosing->get(name);
        }

        throw std::runtime_error(QString("未定义的变量" + name.lexeme + "'。").toStdString());
    }

    // 从内向外查找赋值
    void assign(const Token& name, const Dynamic& value) {
        if (values.contains(name.lexeme)) {
            values[name.lexeme] = value;
            return;
        }

        if (enclosing) {
            enclosing->assign(name, value);
            return;
        }

        throw std::runtime_error(QString("未定义的变量" + name.lexeme + "'。").toStdString());
    }

    // 获取外层环境
    std::shared_ptr<Environment> getEnclosing() const {
        return enclosing;
    }
};