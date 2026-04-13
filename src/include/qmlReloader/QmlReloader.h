//
// Created by 44224 on 4/10/2026.
//
#pragma once

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QKeyEvent>

// TODO:定义一个简单的事件监听类（太麻烦，已弃用）
class QmlReloader : public QObject {
public:
    QmlReloader(QQmlApplicationEngine *e) : engine(e) {}

protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        if (event->type() == QEvent::KeyPress) {
            auto keyEvent = dynamic_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_F5) {
                qDebug() << "重新加载qml文件...";
                engine->clearComponentCache(); // 清除缓存，否则它会读取旧的文件

                // 找到当前的窗口并关闭它，防止打开多个窗口
                const QList<QObject*> rootObjects = engine->rootObjects();
                for (QObject* obj : rootObjects) {
                    obj->deleteLater();
                }

                // 重新加载主入口，仅限开发，因此我直接硬编码
                engine->load(QUrl(QStringLiteral("/home/kkplay/projects/FNFQt/Friday-Night-Funkin-Qt-Reimplemented/QML/main.qml")));
                return true;
            }
        }
        return QObject::eventFilter(obj, event);
    }

private:
    QQmlApplicationEngine *engine;
};