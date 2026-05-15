//
// Created by kkplay on 5/4/26.
//

#include "data/mod/ModRegistry.h"
#include "utils/Path.h"

void ModRegistry::scanAllModMetadatas()
{
    // 创建模组文件夹！
    Path::checkWhetherModDirExists();
    // 先清空模组列表
    entries.clear();
    modMetadatas.clear();
    // 获得模组的目录！
    QVector<QString> modsPaths = Path::getModSubFolderPaths();
    optional<ModMetadata> modMetadata;
    // 循环获取mod里面的元数据内容！
    int loopTimes = 0;
    for (auto& modPath : modsPaths)
    {
        loopTimes++;
        qInfo() << loopTimes;
        // 自动把数据加到registry里面，无需我们手动添加
        switch(ModScanUtils::judgeModEngine(modPath)){
        case ModEngineType::PE: ModScanUtils::scanPEModMetadata(modPath); break;
        case ModEngineType::VS: ModScanUtils::scanVSModMetadata(modPath); break;
        default: break;
        }
    }

    MessageHandler::logInfo("模组数量为: " + QString::number(modListModel->rowCount()),"ModManager::scanModMetadatas");
    // 模组扫描完成，通知qml刷新界面！
    emit onStateChanged();
}

void ModRegistry::parseAllMods()
{
    if (entries.empty()) scanAllModMetadatas();
    ModScanUtils::scanAllMods(modMetadatas);
}
