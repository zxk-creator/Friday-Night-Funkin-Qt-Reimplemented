//
// Created by kkplay on 4/16/26.
//

#pragma once
#include "data/mod/ModMetadata.h"
#include <QAbstractListModel>
// 类似Haxe的Any类型
#include <QVariant>

class ModRegistry;

/**
 * 要支持显示到qml界面中，我们必须定义这个
 * 他自身也是一个管理类，管理了模组界面显示的所有模组，C++这边只需要调用"addMod"即可添加模组进入内部，自动刷新界面
 * 只接受metadata对象，只适用于模组管理界面。
**/
class ModListModel : public QAbstractListModel
{
    Q_OBJECT

    // 当countChanged函数触发的时候，自动读取rowCount函数获得返回值赋给QML里面的count。
    Q_PROPERTY(int modCount READ rowCount NOTIFY countChanged)

public:
    enum ModRoles
    {
        NameRole = Qt::UserRole + 1,
        IconRole,
        PathRole,
        DetailRole,
    };

    // 相当于super调用父类的函数
    explicit ModListModel(QObject *parent = nullptr) : QAbstractListModel(parent) {}

    // 显示列表中有几个元素？
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    // 专门给qml界面看的！
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Role与QMLJSON属性名的映射
    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[NameRole] = "title";
        roles[IconRole] = "icon";
        roles[PathRole] = "path";
        roles[DetailRole] = "detail";
        return roles;
    }

signals:
    void countChanged();
};
