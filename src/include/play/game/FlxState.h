//
// Created by kkplay on 7/12/26.
//

#pragma once
#include "FlxObject.h"
#include "utils/lang/LangStringPool.h"

class FlxState : public FlxObject
{
public:
    QString name;
    QString QMLFileAbsolutePath;

    FlxState(QString& name,QString& QMLFileAbsolutePath) : name(name),QMLFileAbsolutePath(QMLFileAbsolutePath) {}
};

