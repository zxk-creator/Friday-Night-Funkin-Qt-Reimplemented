//
// Created by 44224 on 3/27/2026.
//

#pragma once
#include "src/include/audio/FunkinSoundSystem.h"
#include "src/include/save/SaveSystem.h"
#include "src/include/utils/GlobalSystemUtils.h"

class InstanceLibrary {
private:
    static inline FunkinSoundSystem* funkinSoundSystem = nullptr;
    static inline SaveSystem* saveSystem = nullptr;
    friend class GlobalSystemUtils;

public:
    // main函数里面一定要记得调用这个！！！！！！！！！！！！！否则崩溃
    static void initSubSystem() {
        saveSystem = new SaveSystem();
        funkinSoundSystem = new FunkinSoundSystem();
    }
};
