//
// Created by kkplay on 7/14/26.
//

#include "Reflect.h"
#include "play/game/FlxSprite.h"
#include "play/states/MenuState.h"

std::shared_ptr<HClass> Reflect::createInstance(const QString& name, std::vector<Dynamic>& args){
    // 新增类的话，直接来这里注册一个new。
    if (name == "TestClass") return std::make_shared<TestClass>();
    if (name == "FlxSprite") return std::make_shared<FlxSprite>();
    if (name == "MenuState") {
    if (args.size() == 2 && args[0].isString() && args[1].isString()) {
            return std::make_unique<MenuState>(args[0].asString(),args[1].asString());
        }
        LOG_ERROR(true,"传给MenuState构造函数的参数数量或类型不正确！");
    }
    if (name == "FlxG") {
        return std::make_unique<FlxG>();
    }

    return nullptr;
}

