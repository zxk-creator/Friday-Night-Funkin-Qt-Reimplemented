#pragma once


#include <qdir.h>

#include "SoundEnum.h"
#include "QString"
#include "utils/GlobalSystemUtils.h"
#include "utils/Path.h"
#include "QDebug"
#include "miniaudio/miniaudio.h"

// 每一个可以在游戏内播放的音效都是一个FunkinSound对象
class FunkinSound
{
public:
    // 声音对象的载体
    ma_sound thisSound;
    QString thisSoundName;
    bool bIsInitialized = false;

    /**
     *
     * @param bLoadToRAM 为true：加载到内存
     * @param filePath 文件路径，支持绝对路径，相对路径，qrc:路径
     * @param soundType 声音类型
     * @param soundName 给这个声音取得名字
     */
    FunkinSound(bool bLoadToRAM,const QString filePath,ESoundType soundType,bool whetherLoop,const QString soundName = "") {

        ma_uint32 flag = 0;
        // 指针，共享同一份实例。一定存在，除非你忘了初始化音频系统
        ma_sound_group* soundGroup = GlobalSystemUtils::getSoundGroupInstance(soundType);
        QString finalPath = Path::finalModPath(filePath);

        if (bLoadToRAM) {
            flag |= MA_SOUND_FLAG_DECODE;
        }

        thisSoundName = soundName;
        ma_result result = ma_sound_init_from_file(GlobalSystemUtils::getSoundEngineInstance(),finalPath.toUtf8().constData(),flag,soundGroup,nullptr,&thisSound);
        if (result != MA_SUCCESS) {
            qCritical() << "声音初始化失败，名称" << thisSoundName << "。原因：" << result;
            return;
        }
        if (whetherLoop)
        {
            ma_sound_set_looping(&thisSound,true);
            qInfo() << thisSoundName << "被设置为重复音效。";
        }
        bIsInitialized = true;
    }
    // 禁止拷贝构造
    FunkinSound(const FunkinSound&) = delete;
    // 也不能赋值
    FunkinSound& operator=(const FunkinSound&) = delete;

    void playSound() {
        if (!bIsInitialized) {
            qCritical() << "声音播放失败，名称" << thisSoundName << "。原因：未初始化音频！";
        }
        // 无论如何，开始播放直接从头开始播放
        ma_sound_seek_to_pcm_frame(&thisSound, 0);
        ma_result result = ma_sound_start(&thisSound);
        if (result != MA_SUCCESS) {
            qCritical() << "声音播放失败，名称" << thisSoundName << "。原因：" << result;
        }
    }

    void stopSound() {
        if (!bIsInitialized) {
            qCritical() << "声音停止失败，名称" << thisSoundName << "。原因：未初始化音频！";
        }
        ma_result result = ma_sound_stop(&thisSound);
        if (result != MA_SUCCESS) {
            qCritical() << "声音停止失败，名称" << thisSoundName << "。原因：" << result;
        }
    }

    ~FunkinSound() {
        if (bIsInitialized) {
            ma_sound_uninit(&thisSound);
        }
    }
};