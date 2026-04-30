//
// Created by kkplay on 4/17/26.
//

#include "modding/modmetadata/ModMetadata.h"
#include "utils/lang/LangStringPool.h"

ModMetadata::ModMetadata()
{
    description = LangStringPool::instance()->noDescription().toStdString();
    title = LangStringPool::instance()->untitledMod().toStdString();
    iconPath = std::nullopt;
}

QString ModMetadata::getMainInfoString() const
{
    QString extraInfoStr;
    auto pool = LangStringPool::instance();

    if (description.has_value())
        extraInfoStr += pool->description() + QString::fromStdString(description.value()) + "<br/>";
    if (contributors.has_value())
        extraInfoStr += pool->contributor() + QString::fromStdString(contributors.value()) + "<br/>";
    if (homepage.has_value())
        extraInfoStr += pool->homepage() + QString::fromStdString(homepage.value()) + "<br/>";

    return extraInfoStr;
}

QString ModMetadata::getExtraInfoString() const
{
    QString extraInfoStr;
    // 简化冗长的代码
    auto pool = LangStringPool::instance();

    if (apiVersion.has_value())
        extraInfoStr += LangStringPool::instance()->modNeedEngineVersion() + QString::fromStdString(apiVersion.value()) + "<br/>";
    if (modVersion.has_value())
        extraInfoStr += LangStringPool::instance()->modVersion() + QString::fromStdString(modVersion.value()) + "<br/>";
    if (dependencies.has_value())
    {
        extraInfoStr += "<br/>" + pool->modOptionalDependencies();
        for (auto [name,rule] : dependencies.value().asKeyValueRange())
        {
            extraInfoStr += QString::fromStdString(name) + "  " + pool->version() + QString::fromStdString(rule) + "<br/>";
        }
    }
    if (optionalDependencies.has_value())
    {
        extraInfoStr += "<br/>" + pool->modOptionalDependencies();
        for (auto [name,rule] : optionalDependencies.value().asKeyValueRange())
        {
            extraInfoStr += QString::fromStdString(name) + "  " + pool->version() + QString::fromStdString(rule) + "<br/>";
        }
    }

    return extraInfoStr;
}
