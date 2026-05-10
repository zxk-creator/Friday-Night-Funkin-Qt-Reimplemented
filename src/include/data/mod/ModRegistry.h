//
// Created by kkplay on 5/3/26.
//

#pragma once
#include "data/BaseRegistry.h"
#include "modding/ModListModel.h"
#include "utils/message/MessageHandler.h"
#include "utils//exception/CustomException.h"

/**
 * 用于管理所有已加载模组，所有内容全都存储在BaseRegistry的entries中。
 */
class ModRegistry : public QObject, public BaseRegistry<ModRegistry>
{
    Q_OBJECT

public:
    // 触发时机: main中initQMLEngine主动构造这个
    ModRegistry() : QObject(nullptr), BaseRegistry("ModRegistry") {
        modListModel = new ModListModel();
        if (!modListModel) Exception::killGame("初始化modListModel失败，清理内存后打开游戏重试。","ModRegistry::new");
        scanAllModMetadatas();
    }

    /**
     * READ指定getter函数即下面的model，CONSTANT表示只会指向这一个地方！用来给QML直接读取用的
     * QML界面只需要ModRegistry(main.cpp中注册的名字).model即可获得我们的ModList！
    **/
    Q_PROPERTY(ModListModel*   modList   READ   getModList NOTIFY onStateChanged)
    //             类型     qml: .modList   读    C++函数名

    Q_INVOKABLE void scanAllModMetadatas();
    /**
     * 调用进行ModScanUtils进行扫描模组
     */
    void parseAllMods();

    /**
     * 扫描某个单个模组
     * @param id 模组所在的文件夹名字
     */
    void parseOneMod(const QString& id){
        auto it = entries.find(id);
        if (it == entries.end()){
            MessageHandler::logWarning("在ModRegistry中找不到id" + id + "，已忽略。\n","ModRegistry");
            return;
        }

        auto derived = static_cast<ModMetadata*>(it->second.get());
        ModScanUtils::scanOneMod(*derived);
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
            MessageHandler::logWarning("警告，传入的entry不是ISerializable的子类。", "ModRegistry::addEntry");
        }

        emit onStateChanged();
    }

    QVector<ModMetadata> modMetadatas;

    // QML调用辅助函数（具体用途在上述Q_OBJECT被引用）
    ModListModel* getModList() const { return modListModel; }

signals:
    // 用于告知QML界面变了，赶紧刷新。
    void onStateChanged();
protected:
    ~ModRegistry() override = default;

    // C++特色: 不初始化，就会导致各种野指针空指针等难以调试的问题
    ModListModel* modListModel = nullptr;
};


