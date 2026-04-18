//
// Created by 44224 on 3/27/2026.
//

#pragma once

#include "../thirdParty/miniaudio/miniaudio.h"
#include "audio/SoundEnum.h"

// 解耦代理类，防止外部依赖特定文件导致重构困难
class GlobalSystemUtils {
public:
    // 修改子系统音量的同时也会写入到保存文件中
    static void setSoundVolume(float newVolume, ESoundType soundType);

    // 从存储实例中获取，最权威
    static float getSoundVolume(ESoundType soundType);

    // 获得声音引擎，全局唯一。
    static ma_engine* getSoundEngineInstance();

    static ma_sound_group* getSoundGroupInstance(ESoundType soundType);
};
