//
// Created by kkplay on 7/12/26.
//

#pragma once
#include "FlxObject.h"
#include "HaxeParser/type/HClass.h"

// 相当于一个关卡对象，所有可见游戏对象都在这里
class FlxState : public FlxObject, public HClass
{
public:
    QString stateName;
    QString QMLFileAbsolutePath;

    FlxState(const QString& stateName,const QString& QMLFileAbsolutePath) : HClass(stateName,nullptr),stateName(stateName), QMLFileAbsolutePath(QMLFileAbsolutePath) {}
};

