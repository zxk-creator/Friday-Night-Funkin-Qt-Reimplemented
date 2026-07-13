#include "utils/GlobalSystemUtils.h"
#include "data/Context.h"
#include "audio/FunkinSoundSystem.h"

float GlobalSystemUtils::getSoundVolume(ESoundType soundType)
{
    auto* sys = Context::soundSystem;
    if (!sys) return 1.0f;

    switch (soundType)
    {
    case ESoundType::uiSound: return sys->uiSoundVolume;
    case ESoundType::vocal:   return sys->vocalSoundVolume;
    case ESoundType::inst:    return sys->instSoundVolume;
    default:                  return 1.0f;
    }
}

ma_engine* GlobalSystemUtils::getSoundEngineInstance()
{
    auto* sys = Context::soundSystem;
    return sys ? &sys->engine : nullptr;
}

ma_sound_group* GlobalSystemUtils::getSoundGroupInstance(ESoundType soundType)
{
    auto* sys = Context::soundSystem;
    if (!sys) return nullptr;

    switch (soundType)
    {
    case ESoundType::uiSound: return &sys->uiSoundGroup;
    case ESoundType::vocal:   return &sys->vocalSoundGroup;
    case ESoundType::inst:    return &sys->instSoundGroup;
    default:                  return nullptr;
    }
}
