//
// Created by kkplay on 5/7/26.
//

#pragma once
#include <cstdlib>

// 若要显示到FNF的世界中以及获取钩子方法，必须继承自这个，而且构造必须使用注册表提供的方法，析构调用自身的方法
class FNFObject {
public:

    /**
     * 标志位: 用于注册表类自动释放内存
     */
    bool isValid = false;

    FNFObject() {
        isValid = true;
    }

    // 必须通过这个进行释放内存，否则就会导致崩溃。
    void destroy() {
        isValid = false;
        onDestroy();
    }

    // 因此您依旧可以直接new，而不用故意写别的方法
    void* operator new(size_t size) {

    }
    void operator delete(void*) {}

    virtual void beginPlay() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void onDestroy() = 0;

    // 禁止外部删除，除非友元
protected:
    virtual ~FNFObject() = default;
};