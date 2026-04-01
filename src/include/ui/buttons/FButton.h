#pragma once
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext> // 用于把 C++ 对象传给 QML
#include <iostream>
#include "QObject.h"

class FButton : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE void onClicked()
	{
		std::cout << "我被点了！" << std::endl;
	}
};