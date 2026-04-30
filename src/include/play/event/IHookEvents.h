//
// Created by kkplay on 4/30/26.
//

#pragma once
#include <vector>
#include "save/SaveSystem.h"
#include "audio/FunkinSoundSystem.h"

#define DECLARE_AUTO_INIT(ClassName)  \
ClassName::instance()

// TODO:实现这个接口，自动获得很多钩子函数（不过我们C++没有反射，所以这种方式似乎不太行？）
class IHookEvents
{
    std::vector<void*> regedMembers;
public:
    // 所以还是手动档初始化吧
    static void onWindowOpen()
    {
        SaveSystem::instance();
        FunkinSoundSystem::instance();
    }

    virtual ~IHookEvents() = default;
};