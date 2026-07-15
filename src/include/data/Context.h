//
// Created by kkplay on 5/6/26.
//

#pragma once
#include "audio/FunkinSoundSystem.h"
#include "save/SaveSystem.h"
#include "data/mod/ModRegistry.h"
#include "HaxeParser/ast/Interpreter.h"
#include "play/GameWindow.h"
#include "play/game/FlxG.h"
#include "utils/Path.h"
#include "utils/lang/LangStringPool.h"

/**
 * 全局单例，持有所有子系统指针。
 * RegistryHub::instance() 持有所有解析后的数据，
 * ModRegistry 仅作 QML 桥接。
 */
class Context
{
public:
    static inline SaveSystem* saveSystem;
    static inline FunkinSoundSystem* soundSystem;
    static inline ModRegistry* modRegistry;
    static inline PathUtil* pathUtil;
    static inline LogProxy* logProxy;
    static inline LangStringPool* lang;
    static inline File* fileutil;
    static inline GameWindow* gameWindow;
    static inline Interpreter* interpreter;
    static inline FlxG* _FlxG;

    Context()
    {
        saveSystem = new SaveSystem();
        soundSystem = new FunkinSoundSystem();
        soundSystem->initBuildInSounds();
        modRegistry = new ModRegistry();
        pathUtil = new PathUtil();
        logProxy = new LogProxy();
        lang = new LangStringPool();
        fileutil = new File();
        _FlxG = new FlxG();
    }
};
