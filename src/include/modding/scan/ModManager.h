//
// Created by kkplay on 4/16/26.
//

#pragma once
#include <QQmlApplicationEngine>
#include "utils/modding/ModScanUtils.h"

// 避免沟槽的循环依赖问题
class ModListModel;

// 真正管理模组，扫描模组，存放扫描结果的
class ModManager : public QObject
{
    Q_OBJECT

    /**
     * READ指定getter函数即下面的model，CONSTANT表示只会指向这一个地方！用来给QML直接读取用的
     * QML界面只需要ModManager(main中你注册的名字).model即可获得我们的ModList！
    **/
    Q_PROPERTY(ModListModel* model READ model CONSTANT)

    /**
     * 等完全拿到数据后再更新qml界面！
     */
    Q_PROPERTY(bool isReady READ isReady NOTIFY onStateChanged)

private:
    static ModManager* m_instance;

public:
    /**
     * 我看了半天也没看明白这到底是想干嘛
     * 可能是C++特有的怪异构造函数巴。作用应该是创建对象实例化我们最下面的private成员m_model！
     * 具体实现在.cpp文件中
    **/
    explicit ModManager(QObject *parent = nullptr);

    static ModManager* instance()
    {
        if (!m_instance) m_instance = new ModManager();
        return m_instance;
    }

    /** 扫描路径下的模组文件！每一个文件夹都是一个潜在的“模组”对象!
      * 我们只需要扫描mods目录下的文件夹，每一个文件夹都赋予一个Mod对象并配置ModMetadata即可！
      * 无论你是Psych Engine还是官方引擎，都能兼容！
      * 其他的引擎，暂时现不管了！
    **/
    Q_INVOKABLE void scanModMetadatas();

    // qml获取数据的辅助函数！
    bool isReady() const { return m_isReady; }
    // 为什么我不直接把m_model设置为public呢非要写个getter
    ModListModel* model() const { return m_model; }

    QVector<ModMetadata> getAllModMetadatas();

    signals:
    void onStateChanged();

private:
    // 初始化放在.cpp文件中了
    ModListModel *m_model;
    bool m_isReady = false;
};