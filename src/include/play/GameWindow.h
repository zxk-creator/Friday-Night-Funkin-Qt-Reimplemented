//
// Created by kkplay on 7/13/26.
//

#pragma once
#include <QQuickView>

#include "game/FlxObject.h"
#include "utils/message/MessageHandler.h"

// 代表了游戏窗口，拥有一切渲染钩子，管理所有游戏对象，发送钩子事件。
class GameWindow : public QQuickView
{
    Q_OBJECT

    QVector<QPointer<QObject>> registeredGameObjects;

public:
    GameWindow(QWindow* parent = nullptr) : QQuickView(parent) {}

    GameWindow(QVector<QPointer<QObject>>& gameObjects,QWindow* parent = nullptr) : QQuickView(parent)
    {
        connect(this, &QQuickWindow::beforeSynchronizing, this, &GameWindow::handleBeforeSync,Qt::DirectConnection);
        registeredGameObjects = gameObjects;
        lastFrameTime = std::chrono::steady_clock::now();
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
        }
    }
private slots:
    // 此处更新数据！相当于每帧循环
    void handleBeforeSync();

private:
    std::chrono::steady_clock::time_point lastFrameTime;
};
