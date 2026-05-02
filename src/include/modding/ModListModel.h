//
// Created by kkplay on 4/16/26.
//

#pragma once
#include "modmetadata/ModMetadata.h"
#include <QAbstractListModel>
// 类似Haxe的Any类型
#include <QVariant>

/**
 * 要支持显示到qml界面中，我们必须定义这个
 * 他自身也是一个管理类，管理了模组界面显示的所有模组，C++这边只需要调用"addMod"即可添加模组进入内部，自动刷新界面
 * 只接受metadata对象，只适用于模组管理界面。
**/
class ModListModel : public QAbstractListModel
{
    Q_OBJECT
    // 使用观察者模式，防止页面进入成功了模组还没有解析完成！
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

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

    // 外部调用这个，直接就把模组添加到这个列表里面！
    void addMod(const ModMetadata &mod) {
        // 发暗号：我要开始添加数据了！
        beginInsertRows(QModelIndex(), m_mods.count(), m_mods.count());
        m_mods << mod;
        // 发暗号：我结束添加数据了，刷新界面！
        endInsertRows();

        // 通知QML模组数量变了！
        emit countChanged();
    }

    // 清空模组列表
    void removeAllMods()
    {
        if (m_mods.isEmpty()) return;
        beginResetModel();
        m_mods.clear();
        endResetModel();
        emit countChanged();
    }

    // 必须重写的虚函数
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return m_mods.count();
    }

    // 专门给qml界面看的！
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (index.row() < 0 || index.row() >= m_mods.count()) return {};

        const ModMetadata &mod = m_mods[index.row()];
        // 把一些常用属性给拼接成字符串方便显示！
        if (role == NameRole && mod.title.has_value()) return mod.title.value();
        if (role == IconRole && mod.iconPath.has_value()) return mod.iconPath.value();
        // 直接返回完整字符串让qml显示！
        if (role == DetailRole) return mod.getDetailString();
        if (role == PathRole) return mod.modPath;

        return {};
    }

    // Role与QMLJSON属性名的映射
    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[NameRole] = "title";
        roles[IconRole] = "icon";
        roles[PathRole] = "path";
        roles[DetailRole] = "detail";
        return roles;
    }

    QVector<ModMetadata> getAllModMetadata()
    {
        return m_mods;
    }

    signals:
    void countChanged();
    
private:
    /**
     * 这里面有几个元素，就会渲染几个ModItem到qml ListView界面上
     * 表示的是目录下一共有几个模组。
    */
    QVector<ModMetadata> m_mods;
};
