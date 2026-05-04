//
// Created by kkplay on 4/17/26.
//

#include "../../../include/data/mod/ModMetadata.h"
#include "utils/lang/LangStringPool.h"

ModMetadata::ModMetadata()
{
    description = LangStringPool::instance()->noDescription();
    title = LangStringPool::instance()->untitledMod();
    iconPath = std::nullopt;
}

QString ModMetadata::getMainInfoString() const
{
    QString extraInfoStr;
    auto pool = LangStringPool::instance();

    if (description.has_value())
        extraInfoStr += pool->description() + description.value() + "<br/>";
    if (contributors.has_value())
        extraInfoStr += pool->contributor() + contributors.value() + "<br/>";
    if (homepage.has_value())
        extraInfoStr += pool->homepage() + homepage.value() + "<br/>";

    return extraInfoStr;
}

QString ModMetadata::getExtraInfoString() const
{
    QString extraInfoStr;
    // 简化冗长的代码
    auto pool = LangStringPool::instance();

    if (apiVersion.has_value())
        extraInfoStr += LangStringPool::instance()->modNeedEngineVersion() + apiVersion.value() + "<br/>";
    if (modVersion.has_value())
        extraInfoStr += LangStringPool::instance()->modVersion() + modVersion.value() + "<br/>";
    if (dependencies.has_value())
    {
        extraInfoStr += "<br/>" + pool->modOptionalDependencies();
        for (auto [name,rule] : dependencies.value().asKeyValueRange())
        {
            extraInfoStr += name + "  " + pool->version() + rule + "<br/>";
        }
    }
    if (optionalDependencies.has_value())
    {
        extraInfoStr += "<br/>" + pool->modOptionalDependencies();
        for (auto [name,rule] : optionalDependencies.value().asKeyValueRange())
        {
            extraInfoStr += name + "  " + pool->version() + rule + "<br/>";
        }
    }

    return extraInfoStr;
}
