//
// Created by 44224 on 3/27/2026.
//

#pragma once
#include "Save.h"
#include "audio/SoundEnum.h"

// 专门用来操作Save类，保存游戏的接口，只负责保存，不负责别的
class SaveSystem{

private:
    Save* saveInstance = nullptr;
    static inline SaveSystem* s_instance;

public:
    // 初始化逻辑写在构造函数里面，new出来后，指针将被一个专门存放的类持有，从而调用他身上的方法
    SaveSystem() {
        // 初始化保存系统
        saveInstance = new Save();
    }

    static SaveSystem* instance()
    {
        if (!s_instance) s_instance = new SaveSystem();
        return s_instance;
    }

protected:
    ~SaveSystem() = default;

public:
    // 这个setSoundVolume只负责写入保存文件
    // 真正修改FunkinSoundSystem的依赖的是工具类里面的一个静态函数，他会调用这个写入文件
    void setSoundVolume(float newVolume, ESoundType soundType) {
        switch (soundType) {
            case ESoundType::uiSound: saveInstance->uiSoundVolume = newVolume; break;
            case ESoundType::vocal: saveInstance->vocalSoundVolume = newVolume; break;
            case ESoundType::inst: saveInstance->instSoundVolume = newVolume; break;
        }
    }

    // 从存储系统中获取音量
    float getSoundVolume(ESoundType soundType) const {
        switch (soundType) {
            case ESoundType::uiSound: return saveInstance->uiSoundVolume;
            case ESoundType::vocal: return saveInstance->vocalSoundVolume;
            case ESoundType::inst: return saveInstance->instSoundVolume;
        }
        // 传错参数返回1
        return 1;
    }
};
