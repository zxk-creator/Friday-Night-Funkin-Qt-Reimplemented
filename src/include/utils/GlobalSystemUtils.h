//
// Created by 44224 on 3/27/2026.
//

#pragma once

#include "InstanceLibrary.h"
// 解耦代理类，防止外部依赖特定文件导致重构困难
class GlobalSystemUtils {
public:
    // 修改子系统音量的同时也会写入到保存文件中
    static void setSoundVolume(float newVolume, ESoundType soundType) {
        newVolume = Math::Clamp(newVolume, 0.0f, 1.5f);
        InstanceLibrary::funkinSoundSystem->setSoundVolume(newVolume,soundType);
        InstanceLibrary::saveSystem->setSoundVolume(newVolume,soundType);
    }

    // 从存储实例中获取，最权威
    static float getSoundVolume(ESoundType soundType) {
        assert(InstanceLibrary::funkinSoundSystem != nullptr && "保存系统未初始化！请查看是否初始化了保存系统，然后才能获得音量大小！");
        return InstanceLibrary::saveSystem->getSoundVolume(soundType);
    }

    // 获得声音引擎，全局唯一。
    static ma_engine* getSoundEngineInstance() {
        assert(InstanceLibrary::funkinSoundSystem != nullptr && "声音系统未初始化！请查看是否初始化了声音系统，然后才能播放声音！");

        return &InstanceLibrary::funkinSoundSystem->engine;
    }

    static ma_sound_group* getSoundGroupInstance(ESoundType soundType) {
        assert(InstanceLibrary::funkinSoundSystem != nullptr && "声音系统未初始化！请查看是否初始化了声音系统，然后才能播放声音！");

        switch (soundType) {
            case ESoundType::inst: {
                return &InstanceLibrary::funkinSoundSystem->instSoundGroup;
            }
            case ESoundType::uiSound: {
                return &InstanceLibrary::funkinSoundSystem->uiSoundGroup;
            }
            case ESoundType::vocal: {
                return &InstanceLibrary::funkinSoundSystem->vocalSoundGroup;
            }
        }

        // 理论上不会执行到这里，除非你传错了参数，因此我们随机返回一个
        return &InstanceLibrary::funkinSoundSystem->uiSoundGroup;
    }
};
