//
// Created by kkplay on 5/3/26.
// Refactored: removed BaseRegistry, now wraps RegistryHub.
//

#pragma once
#include <QObject>
#include <QVector>

#include "modding/ModListModel.h"
#include "data/mod/ModMetadata.h"
#include "data/registry/RegistryHub.h"
#include "utils/message/MessageHandler.h"
#include "utils/exception/CustomException.h"
#include "utils/file/FileUtil.h"

// 临时缓冲区，给模组详情页面看的
struct ModInfoCache
{
    QString levelRelative;
    QString songRelative;
    QString noteStyleRelative;
    QString characterRelative;
    QString stageRelative;
};

/**
 * ModRegistry — QML 桥接层
 *
 * 纯粹作为 QML 与底层扫描器/注册表之间的桥梁。
 * 所有数据存储在 RegistryHub 中，这里只负责：
 *   1. 接收 QML 的扫描请求 → 转发给 ModScanner
 *   2. 暴露 ModListModel 给 QML 的 ListView
 *   3. 提供缓存信息（详情页）
 */
class ModRegistry : public QObject
{
    Q_OBJECT

public:
    explicit ModRegistry(QObject* parent = nullptr)
        : QObject(parent)
    {
        modListModel = new ModListModel();
        if (!modListModel)
            Exception::killGame("初始化modListModel失败", "ModRegistry::new");
    }

    ModInfoCache* getCache() { return &modInfoCache; }

    Q_INVOKABLE QString getCacheContent()
    {
        return QString("%1\n%2\n%3\n%4\n%5\n")
            .arg(modInfoCache.levelRelative)
            .arg(modInfoCache.songRelative)
            .arg(modInfoCache.noteStyleRelative)
            .arg(modInfoCache.characterRelative)
            .arg(modInfoCache.stageRelative);
    }

    Q_PROPERTY(ModListModel* modList READ getModList NOTIFY onStateChanged)

    Q_INVOKABLE void scanAllModMetadatas();
    Q_INVOKABLE void parseAllMods();

    Q_INVOKABLE void parseOneMod(const QString& modIdOrPath);

    void clearAllEntries();

    ModListModel* getModList() const { return modListModel; }

signals:
    void onStateChanged();
    void modParsed();

private:
    ModInfoCache modInfoCache;
    ModListModel* modListModel = nullptr;
};
