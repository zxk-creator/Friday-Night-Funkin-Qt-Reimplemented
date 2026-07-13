//
// Created by kkplay on 7/12/26.
//

#pragma once
#include <QQuickItem>

#include "FlxObject.h"

class FlxText : public QQuickItem
{
    Q_OBJECT

    QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*) override
    {
    }
};
