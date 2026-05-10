//
// Created by kkplay on 4/17/26.
//

#include "utils/GlobalSystemUtils.h"
#include "utils/Math.h"
#include "audio/SoundEnum.h"
#include "data/Context.h"
#include "save/SaveSystem.h"

void GlobalSystemUtils::setSoundVolume(float newVolume, ESoundType soundType)
{
    newVolume = Math::Clamp(newVolume, 0.0f, 1.5f);
    Context::soundSystem->setSoundVolume(newVolume,soundType);
    Context::saveSystem->setSoundVolume(newVolume,soundType);
}

float GlobalSystemUtils::getSoundVolume(ESoundType soundType)
{
    assert(Context::saveSystem != nullptr && "保存系统未初始化！请查看是否初始化了保存系统，然后才能获得音量大小！");
    return Context::saveSystem->getSoundVolume(soundType);
}

ma_engine* GlobalSystemUtils::getSoundEngineInstance()
{
    assert(Context::soundSystem != nullptr && "声音系统未初始化！请查看是否初始化了声音系统，然后才能播放声音！");
    return &Context::soundSystem->engine;
}

ma_sound_group* GlobalSystemUtils::getSoundGroupInstance(ESoundType soundType)
{
    assert(Context::soundSystem != nullptr && "声音系统未初始化！请查看是否初始化了声音系统，然后才能播放声音！");

    switch (soundType) {
    case ESoundType::inst: {
            return &Context::soundSystem->instSoundGroup;
    }
    case ESoundType::uiSound: {
            return &Context::soundSystem->uiSoundGroup;
    }
    case ESoundType::vocal: {
            return &Context::soundSystem->vocalSoundGroup;
    }
    }

    // 理论上不会执行到这里，除非你传错了参数，因此我们随机返回一个
    return &Context::soundSystem->uiSoundGroup;
}
