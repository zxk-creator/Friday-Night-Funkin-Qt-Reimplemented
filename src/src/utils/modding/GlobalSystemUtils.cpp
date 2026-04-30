//
// Created by kkplay on 4/17/26.
//

#include "utils/GlobalSystemUtils.h"
#include "utils/Math.h"
#include "audio/SoundEnum.h"
#include "audio/FunkinSoundSystem.h"
#include "save/SaveSystem.h"

void GlobalSystemUtils::setSoundVolume(float newVolume, ESoundType soundType)
{
    newVolume = Math::Clamp(newVolume, 0.0f, 1.5f);
    FunkinSoundSystem::instance()->setSoundVolume(newVolume,soundType);
    SaveSystem::instance()->setSoundVolume(newVolume,soundType);
}

float GlobalSystemUtils::getSoundVolume(ESoundType soundType)
{
    assert(SaveSystem::instance() != nullptr && "保存系统未初始化！请查看是否初始化了保存系统，然后才能获得音量大小！");
    return SaveSystem::instance()->getSoundVolume(soundType);
}

ma_engine* GlobalSystemUtils::getSoundEngineInstance()
{
    assert(FunkinSoundSystem::instance() != nullptr && "声音系统未初始化！请查看是否初始化了声音系统，然后才能播放声音！");
    return &FunkinSoundSystem::instance()->engine;
}

ma_sound_group* GlobalSystemUtils::getSoundGroupInstance(ESoundType soundType)
{
    assert(FunkinSoundSystem::instance() != nullptr && "声音系统未初始化！请查看是否初始化了声音系统，然后才能播放声音！");

    switch (soundType) {
    case ESoundType::inst: {
            return &FunkinSoundSystem::instance()->instSoundGroup;
    }
    case ESoundType::uiSound: {
            return &FunkinSoundSystem::instance()->uiSoundGroup;
    }
    case ESoundType::vocal: {
            return &FunkinSoundSystem::instance()->vocalSoundGroup;
    }
    }

    // 理论上不会执行到这里，除非你传错了参数，因此我们随机返回一个
    return &FunkinSoundSystem::instance()->uiSoundGroup;
}
