//
// Created by kkplay on 5/3/26.
//

#pragma once
#include "data/BaseRegistry.h"
#include "modding/ModListModel.h"
#include "utils/message/MessageHandler.h"
#include "utils//exception/CustomException.h"
#include "utils/file/FileUtil.h"

// 相当于临时缓冲区，用来给模组详情页面看的
struct ModInfoCache
{
    QString levelRelative;
    QString songRelative;
    QString noteStyleRelative;
    QString characterRelative;
    QString stageRelative;
};

/**
 * 用于管理所有已加载模组，所有内容全都存储在BaseRegistry的entries中。
 */
class ModRegistry : public QObject, public BaseRegistry<ModRegistry>
{
    Q_OBJECT

protected:
    ModInfoCache modInfoCache;

public:
    // 触发时机: main中initQMLEngine主动构造这个
    ModRegistry() : QObject(nullptr), BaseRegistry("ModRegistry") {
        modListModel = new ModListModel();
        if (!modListModel) Exception::killGame("初始化modListModel失败，清理内存后打开游戏重试。","ModRegistry::new");
    }

    /**
     * 获取缓冲区用于给QML界面显示使用
     * @return 缓冲区引用（一定有效）
     */
    ModInfoCache* getCache()
    {
        return &modInfoCache;
    }

    Q_INVOKABLE QString getCacheContent()
    {

        QString info = QString("%1\n%2\n%3\n%4\n%5\n")
            .arg(modInfoCache.levelRelative)
            .arg(modInfoCache.songRelative)
            .arg(modInfoCache.noteStyleRelative)
            .arg(modInfoCache.characterRelative)
            .arg(modInfoCache.stageRelative);
        LOG_INFO("缓存信息为: " + info);
        return info;
    }

    /**
     * READ指定getter函数即下面的model，CONSTANT表示只会指向这一个地方！用来给QML直接读取用的
     * QML界面只需要ModRegistry(main.cpp中注册的名字).model即可获得我们的ModList！
    **/
    Q_PROPERTY(ModListModel*   modList   READ   getModList NOTIFY onStateChanged)
    //             类型     qml: .modList   读    C++函数名

    // 不自动调用，而是手动调用！
    Q_INVOKABLE void scanAllModMetadatas();
    /**
     * 调用进行ModScanUtils进行扫描模组
     */
    Q_INVOKABLE void parseAllMods();

    /**
     * 扫描某个单个模组
     * @param id 模组所在的文件夹名字或绝对路径
     */
    Q_INVOKABLE void parseOneMod(const QString& idOrPath) {

        std::optional<QString> res = FileUtil::getPathLeaf(idOrPath);
        if (!res.has_value())
        {
            LOG_WARNING(false,"尝试根据id获取模组名时出现问题，可能是id" + idOrPath + "不存在。");
            return;
        }

        auto it = entries.find(res.value());
        if (it == entries.end()){
            MessageHandler::logWarning(false,"在ModRegistry中找不到" + idOrPath+ "，已忽略。\n","ModRegistry");
            return;
        }

        auto derived = static_cast<ModMetadata*>(it->second.get());
        ModScanUtils::scanOneMod(*derived);

        emit modParsed();
    }

    void clearAllEntries() override {
        BaseRegistry::clearAllEntries();
        modMetadatas.clear();
        emit onStateChanged();
    }

    void addEntry(const QString& id, unique_ptr<ISerializable> entry) override {
        if (!entry) return;
        if (auto* modMeta = dynamic_cast<ModMetadata*>(entry.get())) {
            modMetadatas.push_back(*modMeta);
            BaseRegistry::addEntry(id, std::move(entry));
        }
        else{
            MessageHandler::logWarning(false,"警告，传入的entry不是ISerializable的子类。", "ModRegistry::addEntry");
        }

        emit onStateChanged();
    }

    QVector<ModMetadata> modMetadatas;

    // QML调用辅助函数（具体用途在上述Q_OBJECT被引用）
    ModListModel* getModList() const { return modListModel; }

signals:
    // 用于告知QML界面变了，赶紧刷新。
    void onStateChanged();
    // 用于解析完成后发的信号
signals:
    void modParsed();

protected:
    ~ModRegistry() override = default;

    // C++特色: 不初始化，就会导致各种野指针空指针等难以调试的问题
    ModListModel* modListModel = nullptr;
};


