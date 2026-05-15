#pragma once
#include <QObject>
#include <QDebug>
#include <QElapsedTimer>

#include "FNFObject.h"

class ObjectRegistry {

};

// TODO: 尚未实现游戏主循环，后面实现。
// DeltaTime聚集地
class GameLoopRegistry : public QObject {
    Q_OBJECT
public:
    explicit GameLoopRegistry(QObject *parent = nullptr) : QObject(parent) {}

    // 默认锁200帧
    static inline int TARGET_FPS = 200;
    // 加入到这个数组里面，才能自动调用Hook方法。
    static inline QVector<FNFObject*> callbacks;

    void registerClass(){

    }

    // 我们暂时先不用多线程方式处理逻辑，配合FrameAnimation，实现每帧处理的效果！
    Q_INVOKABLE void processTick(float deltaTime) {
        for (int i = callbacks.size() - 1;i >= 0; i--){
            // 说明存活
            if (callbacks[i]->isValid){
                callbacks[i]->Update(deltaTime);
            }
            // 说明对象已释放，移除
            else{
                // delete callbacks[i];
                callbacks.removeAt(i);
            }
        }
    }
};

// 定义一个宏用来自动注册类名！
#define REGISTER_CLASS(clsName) /
