//
// Created by kkplay on 7/13/26.
//

#pragma once
#include <QQuickView>

#include "game/FlxObject.h"
#include "game/FlxState.h"
#include "HaxeParser/type/HClass.h"
#include "utils/message/MessageHandler.h"
#include "TypeAlias.h"
#include "utils/exception/CustomException.h"

// 代表了游戏窗口，游戏世界，拥有一切渲染钩子，管理所有游戏对象，发送钩子事件。
class FlxG : public QQuickView, public HClass
{
    Q_OBJECT

    QVector<QPointer<QObject>> registeredGameObjects;

    SharedPtr<FlxState> currentState = nullptr;

public:
    FlxG() : QQuickView(nullptr), HClass("FlxG", nullptr) {
        connect(this, &QQuickWindow::beforeSynchronizing, this, &FlxG::handleBeforeSync,Qt::DirectConnection);
        lastFrameTime = std::chrono::steady_clock::now();
    }

    void init()
    {
        for (const auto& obj : registeredGameObjects)
        {
            qobject_cast<FlxObject*>(obj.data())->create();
        }

        registerNativeMethod("switchState", FunctionType([this](const std::vector<Dynamic>& args) -> Dynamic
        {
            if (args.size() == 1 && args[0].isObject()) {
                auto obj = std::static_pointer_cast<FlxState>(args[0].asObject());
                if (obj) {
                    this->switchState(obj);
                }
            }

            LOG_SCRIPT_WRONG_PARAM_ERROR();
            return {};
        }));
    }

    void registerNewGameObjects(const QVector<QPointer<QObject>>& gameObjects)
    {
        for (auto& ptr : gameObjects)
        {
            if (ptr.isNull())
            {
                LOG_WARNING(false,"传入了空指针！");
                continue;
            }
            registeredGameObjects.push_back(ptr);
            qobject_cast<FlxObject*>(ptr.data())->create();
        }
    }

    void removeGameObject(const QString& name) {
        registeredGameObjects.erase(std::ranges::remove_if(registeredGameObjects
                           ,
                           [&](const auto& rawObj) {
                               if (!rawObj) return false;
                               auto obj = qobject_cast<FlxObject*>(rawObj.data());
                               return obj && obj->getName() == name;
                           }
            ).begin(),
    registeredGameObjects.end());
    }

    // 切换关卡（一个全新QML文件）
    void switchState(SharedPtr<FlxState> newState) {
        if (!newState) {
            LOG_NULL_PTR_ERROR("传入了空指针！");
            return;
        }
        for (const auto& rawPtr : registeredGameObjects) {
            if (rawPtr.isNull()) continue;
            qobject_cast<FlxObject*>(rawPtr.data())->kill();
        }

        currentState.reset();
        currentState = newState;
        newState->create();
        setSource(newState->QMLFileAbsolutePath);

    }
private slots:
    // 此处更新数据！相当于每帧循环
    void handleBeforeSync();

private:
    std::chrono::steady_clock::time_point lastFrameTime;
};
