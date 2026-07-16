//
// Created by kkplay on 7/9/26.
//

#pragma once
#include <QMap>
#include <QString>
#include <memory>
#include "BaseClass.h"
#include "../util/ExceptionUtil.h"
#include "Dynamic.h"

class Environment;
class FunctionStmt;
class Dynamic;
class Expr;

// 字段声明信息
struct FieldDecl {
    QString name;
    Expr* initializer = nullptr;
};

/**
 * 对象的类，相当于内存分配规则。C++父类要想被脚本类继承则需要继承自这个类
 * 若要让脚本能够访问和修改C++类的成员，则必须写getPropertyName()函数（PropertyName替换为字段名）
 */
class HClass : public HObject {
public:
    // 类名
    QString clsName;
    // 父类指针，若没有则为nullptr
    std::shared_ptr<HClass> superclass;
    // 脚本方法：所有权在ClassStmt AST节点,{方法名，执行的函数体}
    QMap<QString, FunctionStmt*> methods;
    // C++原生方法
    QMap<QString, FunctionType> nativeMethods;
    // 定义类的时候的环境闭包
    std::shared_ptr<Environment> closure;

    HClass(QString clsName, std::shared_ptr<HClass> superclass)
        : clsName(std::move(clsName)), superclass(std::move(superclass)) {}

    // 字段声明列表，用于在实例化时创建字段并赋初值
    std::vector<FieldDecl> fieldDecls;

    // 下面是静态字段
    // 脚本静态字段，值在类定义时初始化
    QMap<QString, Dynamic> staticFields;
    // 脚本静态方法
    QMap<QString, FunctionStmt*> staticMethods;

    // 查找脚本静态方法
    FunctionStmt* findStaticScriptMethod(const QString& name) {
        if (staticMethods.contains(name)) return staticMethods[name];
        if (superclass) return superclass->findStaticScriptMethod(name);
        return nullptr;
    }

    // 查找静态字段值
    Dynamic* findStaticField(const QString& name) {
        auto it = staticFields.find(name);
        if (it != staticFields.end()) return &it.value();
        if (superclass) return superclass->findStaticField(name);
        return nullptr;
    }

    // 查找要调用的方法，包括脚本方法和C++方法
    std::pair<const FunctionStmt*,FunctionType> findMethod(const QString& name) {
        // 首先查找脚本方法
        if (methods.contains(name)) return std::pair<FunctionStmt*, FunctionType>{methods[name], nullptr};
        // 找不到则再找C++方法
        if (nativeMethods.contains(name)) return std::pair<FunctionStmt*, FunctionType>{nullptr, nativeMethods[name]};
        // 没有？那么找父类的
        if (superclass) return superclass->findMethod(name);
        // 这也找不到，那只能找不到返回空指针。
        return {nullptr,nullptr};
    }

    // 需要传入一个lambda。子类在lambda内捕获this指针，处理参数和返回值。
    void registerNativeMethods(const QMap<QString, FunctionType>& _methods)
    {
        nativeMethods = _methods;
    }

    void registerNativeMethod(const QString& name, FunctionType func)
    {
        nativeMethods[name] = std::move(func);
    }

    // 本质上这也是一个FunctionType，因为他就是一个普通lambda。并非实例方法。因此可以被脚本调用
    Dynamic callNativeMethod(const QString& name, std::vector<Dynamic>& args)
    {
        if (nativeMethods.contains(name)) return nativeMethods[name](args);
        ScriptError::throwRuntimeError(QString("没有找到方法" + name + "!"));
        return {};
    }

    QMap<QString, FunctionType> getAllNativeMethod()
    {
        return nativeMethods;
    }

    // 子类请重写这个，这样通过反射获取到属性时才能正确返回
    virtual Dynamic getField(const QString& fieldName)
    {
        if (superclass) return superclass->getField(fieldName);
        ScriptError::throwNoSuchFieldError(fieldName);
    }
    // 子类请重写这个，这样通过反射设置属性时才能正确设置
    virtual void setField(const QString& fieldName, Dynamic value)
    {
        if (superclass) return superclass->setField(fieldName, value);
        ScriptError::throwNoSuchFieldError(fieldName);
    }

    // 读取的字段包括静态字段
    virtual bool hasField(const QString& fieldName)
    {
        if (superclass) return superclass->hasField(fieldName);
        return false;
    }
};

// 类实例
class HInstance : public HObject {
public:
    // 指向对象所属的类
    std::shared_ptr<HClass> klass;
    // 声明的变量表
    QMap<QString, Dynamic> fields;

    // 参数用于初始化其指向的类
    explicit HInstance(std::shared_ptr<HClass> klass) : klass(std::move(klass)) {}
};
