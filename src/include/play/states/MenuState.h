//
// Created by kkplay on 7/13/26.
//

#pragma
#include "play/game/FlxState.h"

// 主菜单
class MenuState : public FlxState
{
public:
    MenuState(QString& name,QString& QMLFileAbsolutePath) : FlxState(name,QMLFileAbsolutePath)
    {

    }

    void update(float elapsed) override
    {

    }

    ~MenuState() override
    {

    }
};
