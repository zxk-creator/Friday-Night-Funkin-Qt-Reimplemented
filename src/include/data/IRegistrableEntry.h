//
// Created by kkplay on 4/22/26.
//

#pragma once

// 接口：每一个可注册的对象都实现这个才能被加载
class IRegistrableEntry
{
public:
    virtual ~IRegistrableEntry() = default;

    virtual void loadEntries() = 0;
};
