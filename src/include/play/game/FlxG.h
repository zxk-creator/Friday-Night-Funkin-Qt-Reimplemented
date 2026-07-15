//
// Created by kkplay on 7/13/26.
//

#pragma once
#include <memory>

#include "FlxState.h"
#include "data/Context.h"
#include "utils/message/MessageHandler.h"

class FlxG
{
public:
    std::unique_ptr<FlxState> currentLevel;

    void switchState(std::unique_ptr<FlxState> newState)
    {
        if (!newState || newState->QMLFileAbsolutePath.isEmpty())
        {
            LOG_WARNING(false,"传入了空指针或QML路径为空！");
        }

        currentLevel = std::move(newState);
        // 一键切换...
        Context::gameWindow->setSource(newState->QMLFileAbsolutePath);
    }
};
