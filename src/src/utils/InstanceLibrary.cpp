//
// Created by kkplay on 4/16/26.
//

#include "utils/InstanceLibrary.h"
#include "modding/scan/ModManager.h"

void InstanceLibrary::initSubSystems()
{
    saveSystem = new SaveSystem();
    funkinSoundSystem = new FunkinSoundSystem();
    funkinSoundSystem->initBuildInSounds();
    modManager = new ModManager();
    langStringPool = new LangStringPool();
}