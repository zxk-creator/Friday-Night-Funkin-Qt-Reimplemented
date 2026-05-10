//
// Created by kkplay on 5/6/26.
//

#pragma once
#include "audio/FunkinSoundSystem.h"
#include "character/CharacterRegistry.h"
#include "level/LevelRegistry.h"
#include "save/SaveSystem.h"
#include "song/SongRegistry.h"
#include "data/mod/ModRegistry.h"
#include "notestyle/NoteStyleRegistry.h"
#include "stage/StageRegistry.h"
#include "utils/Path.h"
#include "utils/lang/LangStringPool.h"

/**
 * 全局单例类，我再也不用instance获取单例了，谁用谁煞笔，尤其是C++
 * 语言缺陷导致你不得不用这种集体注册的方式
 * 总之，一定要记住main中先new出来这个！
 */
class Context
{
public:
    static inline SaveSystem* saveSystem;
    static inline FunkinSoundSystem* soundSystem;
    static inline CharacterRegistry* characterRegistry;
    static inline LevelRegistry* levelRegistry;
    static inline ModRegistry* modRegistry;
    static inline SongRegistry* songRegistry;
    static inline PathUtil* pathUtil;
    static inline LogProxy* logProxy;
    static inline StageRegistry* stageRegistry;
    static inline NoteStyleRegistry* noteStyleRegistry;
    static inline LangStringPool* lang;

    Context(){
        saveSystem = new SaveSystem();
        soundSystem = new FunkinSoundSystem();
        soundSystem->initBuildInSounds();
        characterRegistry = new CharacterRegistry();
        levelRegistry = new LevelRegistry();
        modRegistry = new ModRegistry();
        songRegistry = new SongRegistry();
        pathUtil = new PathUtil();
        logProxy = new LogProxy();
        stageRegistry = new StageRegistry();
        noteStyleRegistry = new NoteStyleRegistry();
        lang = new LangStringPool();
    }
};