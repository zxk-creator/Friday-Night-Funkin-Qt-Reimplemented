//
// Created by kkplay on 4/17/26.
//

#include "modding/ModListModel.h"
#include "utils/Path.h"
#include "modding/scan/ModManager.h"

/**
 * 为什么用new，因为他这里存的是所有目录下的模组，全局统一，不用考虑内存泄漏问题。
 */
ModManager::ModManager(QObject* parent) : QObject(parent), m_model(new ModListModel(this))
{
    scanModMetadatas();
}

void ModManager::scanModMetadatas()
{
    // 创建模组文件夹！
    Path::checkWhetherModDirExists();
    // 先清空模组！
    m_model->removeAllMods();
    // 获得模组的目录！
    QVector<QString> modsPaths = Path::getAllModFolderPaths();
    optional<ModMetadata> modMetadata = nullopt;
    // 循环获取里面的元数据内容！
    for (auto& modPath : modsPaths)
    {
        switch(ModScanUtils::judgeModEngine(modPath))
        {
        case ModEngineType::PE: ModScanUtils::scanPEModMetadata(modPath); break;
        case ModEngineType::VS: ModScanUtils::scanVSModMetadata(modPath); break;
        default: break;
        }
        if (modMetadata.has_value()) m_model->addMod(modMetadata.value());
    }

    qInfo() << "模组数量为：" << m_model->rowCount();
    // 模组扫描完成，通知qml刷新界面！
    m_isReady = true;
    emit onStateChanged();
}

QVector<ModMetadata> ModManager::getAllModMetadatas()
{
    // 理论上这个东西本身就不应该为空，即便是没有模组也有值
    assert(m_model != nullptr);
    return m_model->getAllModMetadata();
}
