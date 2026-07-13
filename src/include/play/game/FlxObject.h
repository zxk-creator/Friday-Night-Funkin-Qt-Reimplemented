//
// Created by kkplay on 5/7/26.
//

#pragma once
#include <cstdlib>

// 若要显示到FNF的世界中以及获取钩子方法，必须继承自这个，而且构造必须使用注册表提供的方法，析构调用自身的方法
class FlxObject {
public:
    // 代替create
    FlxObject()
    {}

    virtual void update(float elapsed) = 0;

    // 代替kill
    virtual ~FlxObject() = default;
};

Q_DECLARE_INTERFACE(FlxObject,"FlxObject")