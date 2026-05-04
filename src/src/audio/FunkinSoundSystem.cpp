//
// Created by 44224 on 3/28/2026.
//

#include <audio/FunkinSoundSystem.h>
#include "utils/GlobalSystemUtils.h"
#include "audio/FunkinSound.h"

FunkinSoundSystem::FunkinSoundSystem(QObject *parent) : QObject(parent) {
    // 一定要先把SaveSystem new出来，再调用这个！！！！！否则崩溃
    uiSoundVolume = GlobalSystemUtils::getSoundVolume(ESoundType::uiSound);
    vocalSoundVolume = GlobalSystemUtils::getSoundVolume(ESoundType::vocal);
    instSoundVolume = GlobalSystemUtils::getSoundVolume(ESoundType::inst);

    // 首先初始化声音引擎
    ma_engine_init(nullptr,&engine);
    // 再初始化音频组
    ma_sound_group_init(&engine,0,nullptr,&uiSoundGroup);
    ma_sound_group_init(&engine,0,nullptr,&vocalSoundGroup);
    ma_sound_group_init(&engine,0,nullptr,&instSoundGroup);
}

void FunkinSoundSystem::initBuildInSounds() {
    // 再初始化默认音效
    this->backSound = new FunkinSound(true,Path::getDefaultSoundPath(EDefaultSoundType::back),ESoundType::uiSound,false,"取消音效");
    this->scrollSound = new FunkinSound(true,Path::getDefaultSoundPath(EDefaultSoundType::scroll),ESoundType::uiSound,false,"滚动音效");
    this->confirmSound = new FunkinSound(true,Path::getDefaultSoundPath(EDefaultSoundType::confirm),ESoundType::uiSound,false,"确认音效");
    this->infoSound = new FunkinSound(true,Path::sound("boop"),ESoundType::uiSound,false,"提示音音效");
}

void FunkinSoundSystem::setSoundVolume(float newVolume, ESoundType soundType) {
    ma_sound_group* targetModifySoundGroup = nullptr;
    switch (soundType) {
        case ESoundType::uiSound: targetModifySoundGroup = &uiSoundGroup; uiSoundVolume = newVolume; break;
        case ESoundType::vocal: targetModifySoundGroup = &vocalSoundGroup; vocalSoundVolume = newVolume; break;
        case ESoundType::inst: targetModifySoundGroup = &instSoundGroup; instSoundVolume = newVolume; break;
    }
    if (targetModifySoundGroup)
        ma_sound_group_set_volume(targetModifySoundGroup, newVolume);
}

void FunkinSoundSystem::playBuildInSound(EDefaultSoundType soundType) {
    switch (soundType) {
        case EDefaultSoundType::back: {
            backSound->playSound();
        }
        case EDefaultSoundType::confirm: {
            confirmSound->playSound();
        }
        case EDefaultSoundType::scroll: {
            scrollSound->playSound();
        }
    }
}

void FunkinSoundSystem::playconfirmSound()
{
    confirmSound->playSound();
}

void FunkinSoundSystem::playbackSound()
{
    backSound->playSound();
}

void FunkinSoundSystem::playscrollSound()
{
    scrollSound->playSound();
}

void FunkinSoundSystem::playinfoSound()
{
    infoSound->playSound();
}



