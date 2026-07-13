#include "modding/ModListModel.h"
#include "data/registry/RegistryHub.h"

int ModListModel::rowCount(const QModelIndex& parent) const
{
    return RegistryHub::instance().modCount();
}

QVariant ModListModel::data(const QModelIndex& index, int role) const
{
    auto& hub = RegistryHub::instance();
    if (index.row() < 0 || index.row() >= hub.modMetadatas.size())
        return {};

    const auto& mod = hub.modMetadatas[index.row()];
    if (role == NameRole && mod.title.has_value()) return mod.title.value();
    if (role == IconRole && mod.iconPath.has_value()) return mod.iconPath.value();
    if (role == DetailRole) return mod.getDetailString();
    if (role == PathRole) return mod.modPath;

    return {};
}
