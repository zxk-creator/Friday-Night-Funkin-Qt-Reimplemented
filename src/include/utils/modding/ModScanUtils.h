//
// Created by kkplay on 4/16/26.
//

#pragma once
#include <QDir>
#include <modding/modmetadata/ModMetadata.h>

using std::nullopt;

/**
 * @brief 虽然说只想兼容PE和VS，但是万一做了呢？所以暂时保留接口！
*/
enum class ModEngineType
{
    // Psych Engine
    PE,
    // V-Slice
    VS,
    // Kade Engine
    KE,
    // Codename Engine
    CNE,
    // 反正肯定不是V-Slice
    Unknown,
};

namespace ModScanUtils
{
    ModEngineType judgeModEngine(const QString& modAbsolutePath);
    optional<ModMetadata> scanPEMod(QString& modAbsolutePath);
    optional<ModMetadata> scanVSMod(QString& modAbsolutePath);
    /** TODO:先扔这儿了，想做了就做了 **/
    inline optional<ModMetadata> scanKEMod(QString& modAbsolutePath) { return nullopt;}
    inline optional<ModMetadata> scanCNEMod(QString& modAbsolutePath) { return nullopt;}
}
