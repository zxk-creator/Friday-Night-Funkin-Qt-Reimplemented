//
// Created by 44224 on 3/27/2026.
//

#pragma once

// 本类需要实例化才能调用。这样内存中就可以共享同一份实例，序列化到磁盘上后也能保持同步
class Save {
public:
    float uiSoundVolume = 1.0f;
    float vocalSoundVolume = 1.0f;
    float instSoundVolume = 1.0f;

    // 从磁盘上读取文件获得保存的内容，由于我还没想好怎么处理保存格式，因此先返回占位符先
    Save() {
        // 已经初始化了，因此我就不初始化了
    }

    // 禁止删除
protected:
    ~Save() {}
};
