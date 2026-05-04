//
// Created by kkplay on 5/3/26.
//

#pragma once
#include "data/BaseRegistry.h"
#include "modding/scan/ModManager.h"
#include "utils/message/MessageHandler.h"

/**
 * 用于管理所有已加载模组，所有内容全都存储在BaseRegistry的entries中。
 */
class ModRegistry : public BaseRegistry<ModRegistry>
{
    // 那还定义他干甚
public:
    ModRegistry() : BaseRegistry("ModRegistry") {}

    void scanAllModMetadatas(){
        ModManager::instance()->scanModMetadatas();
    }

    /**
     * 调用进行ModScanUtils进行扫描模组
     */
    void parseAllMods(){
        if (entries.empty()) scanAllModMetadatas();

        QVector<ModMetadata> _metadatas;
        for (auto& entry : entries){
            auto derived = static_cast<ModMetadata*>(entry.second.get());
            _metadatas.push_back(*derived);
        }
        ModScanUtils::scanAllMods(_metadatas);
    }

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
protected:
    ~ModRegistry() override = default;
};