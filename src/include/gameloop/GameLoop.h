#pragma once
#include <QObject>
#include <QDebug>

class GameLoop : public QObject {
    Q_OBJECT
public:
    explicit GameLoop(QObject *parent = nullptr) : QObject(parent) {}

    Q_INVOKABLE void processTick(double deltaTime) {

    }
};