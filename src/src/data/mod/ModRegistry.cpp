#include "data/mod/ModRegistry.h"
#include "utils/Path.h"
#include "scanner/ModScanner.h"

void ModRegistry::scanAllModMetadatas()
{
    Path::checkWhetherModDirExists();

    auto& hub = RegistryHub::instance();
    hub.clearAll();

    ModScanner::scanAllMetadatas();

    // 同步回 modListModel 用于 QML 显示
    emit onStateChanged();
}

void ModRegistry::parseAllMods()
{
    auto& hub = RegistryHub::instance();
    if (hub.modMetadatas.isEmpty())
        scanAllModMetadatas();

    ModScanner::scanAllContents();
    emit onStateChanged();
}

void ModRegistry::parseOneMod(const QString& modIdOrPath)
{
    ModScanner::scanOneMod(modIdOrPath);
    emit modParsed();
}

void ModRegistry::clearAllEntries()
{
    RegistryHub::instance().clearAll();
    emit onStateChanged();
}
