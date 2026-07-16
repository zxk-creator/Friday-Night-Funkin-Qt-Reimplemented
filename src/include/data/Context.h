//
// Created by kkplay on 5/6/26.
//

#pragma once
#include "audio/FunkinSoundSystem.h"
#include "save/SaveSystem.h"
#include "data/mod/ModRegistry.h"
#include "HaxeParser/ast/Interpreter.h"
#include "play/FlxG.h"
#include "utils/Path.h"
#include "utils/lang/LangStringPool.h"

/**
 * 全局单例，持有所有子系统指针。
 * RegistryHub::instance() 持有所有解析后的数据，
 * ModRegistry 仅作 QML 桥接。
 */
class Context : public HClass
{
public:
    static inline SaveSystem* saveSystem;
    static inline FunkinSoundSystem* soundSystem;
    static inline ModRegistry* modRegistry;
    static inline PathUtil* pathUtil;
    static inline LogProxy* logProxy;
    static inline LangStringPool* lang;
    static inline File* fileutil;
    static inline Interpreter* interpreter;
    // 这个必须依靠main.cpp中注册！！！！！！！否则为空崩溃
    static inline std::shared_ptr<FlxG> _FlxG;

    // 当注册静态字段时用的
    Context() : HClass("Co0ntext",nullptr) {};

    Context(Interpreter* interpreter) : HClass("Context",nullptr)
    {
        saveSystem = new SaveSystem();
        soundSystem = new FunkinSoundSystem();
        soundSystem->initBuildInSounds();
        modRegistry = new ModRegistry();
        pathUtil = new PathUtil();
        logProxy = new LogProxy();
        lang = new LangStringPool();
        fileutil = new File();
    }

    Dynamic getField(const QString &fieldName) override {
        if (fieldName == "FlxG") return std::shared_ptr<HObject>(_FlxG);

        return {};
    }

    void setField(const QString &fieldName, Dynamic value) override {
        LOG_ERROR(false,"您不应该设置Context中任何内容。");
    }

    bool hasField(const QString& fieldName) override {
        if (fieldName == "FlxG") return true;

        return HClass::hasField(fieldName);
    }
};
