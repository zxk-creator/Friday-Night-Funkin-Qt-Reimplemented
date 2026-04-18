//
// Created by kkplay on 4/17/26.
//

#include "modding/ModListModel.h"
#include "utils/Path.h"
#include "modding/scan/ModManager.h"

ModManager::ModManager(QObject* parent) : QObject(parent), m_model(new ModListModel(this))
{
    scanMods();
}

void ModManager::scanMods()
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
        case ModEngineType::PE: modMetadata = ModScanUtils::scanPEMod(modPath); break;
        case ModEngineType::VS: modMetadata = ModScanUtils::scanVSMod(modPath); break;
        default: break;
        }
        if (modMetadata.has_value()) m_model->addMod(modMetadata.value());
    }

    qInfo() << "模组数量为：" << m_model->rowCount();
    // 模组扫描完成，通知qml刷新界面！
    m_isReady = true;
    emit onStateChanged();
}
