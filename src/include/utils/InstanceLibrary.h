//
// Created by 44224 on 3/27/2026.
//

#pragma once
#include "src/include/audio/FunkinSoundSystem.h"
#include "src/include/save/SaveSystem.h"
#include "src/include/utils/GlobalSystemUtils.h"
#include "utils/lang/LangStringPool.h"
#include "modding/scan/ModManager.h"

/**
 * @brief 集中管理所有子系统，单例模式，直接就能拿来用！
 */
class InstanceLibrary {
public:
    static inline FunkinSoundSystem* funkinSoundSystem = nullptr;
    static inline SaveSystem* saveSystem = nullptr;
    static inline ModManager* modManager = nullptr;
    static inline LangStringPool* langStringPool = nullptr;

    // main函数里面一定要记得调用这个！！！！！！！！！！！！！否则崩溃
    static void initSubSystems();
};
