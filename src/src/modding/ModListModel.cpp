#include "modding/ModListModel.h"

#include "data/Context.h"
#include "data/mod/ModRegistry.h"

int ModListModel::rowCount(const QModelIndex& parent) const
{
    return ModRegistry::entryCount;
}

QVariant ModListModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= ModRegistry::entryCount) return {};

    const ModMetadata &mod = Context::modRegistry->modMetadatas[index.row()];
    if (role == NameRole && mod.title.has_value()) return mod.title.value();
    if (role == IconRole && mod.iconPath.has_value()) return mod.iconPath.value();
    // 直接返回完整字符串让qml显示！
    if (role == DetailRole) return mod.getDetailString();
    if (role == PathRole) return mod.modPath;

    return {};
}
