#include "scanner/ModScanner.h"
#include "scanner/VSScanner.h"
#include "scanner/PEScanner.h"

#include <QDir>
#include <QFileInfo>

#include "utils/file/FileUtil.h"
#include "utils/Path.h"
#include "utils/message/MessageHandler.h"
#include "data/Context.h"
#include "data/registry/RegistryHub.h"

// ==================== 引擎检测 ====================

ModEngineType ModScanner::judgeEngine(const QString& modAbsolutePath)
{
    QDir modDir(modAbsolutePath);

    if (QFile::exists(modDir.filePath("pack.json"))) {
        LOG_INFO(modDir.dirName() + "是PE模组");
        return ModEngineType::PE;
    }

    if (QFile::exists(modDir.filePath("_polymod_meta.json"))) {
        LOG_INFO(modDir.dirName() + "是V-Slice模组");
        return ModEngineType::VS;
    }

    LOG_WARNING(false, modDir.dirName() + "无法识别引擎，默认PE");
    return ModEngineType::PE;
}

// ==================== 元数据扫描 ====================

std::optional<ModMetadata> ModScanner::scanOneMetadata(const QString& modAbsolutePath)
{
    switch (judgeEngine(modAbsolutePath)) {
    case ModEngineType::PE:
        if (auto meta = PEModScanner::scanMetadata(modAbsolutePath))
            return std::move(*meta);
        break;
    case ModEngineType::VS:
        if (auto meta = VSModScanner::scanMetadata(modAbsolutePath))
            return std::move(*meta);
        break;
    default:
        break;
    }
    return std::nullopt;
}

void ModScanner::scanAllMetadatas()
{
    Path::checkWhetherModDirExists();

    auto& hub = RegistryHub::instance();
    hub.clearAll();

    // 确保旧 Context 的 mod registry 也清空
    Context::modRegistry->clearAllEntries();

    QVector<QString> modPaths = Path::getModSubFolderPaths();
    int count = 0;

    for (const auto& modPath : modPaths) {
        auto metaOpt = scanOneMetadata(modPath);
        if (metaOpt) {
            auto meta = std::make_unique<ModMetadata>(std::move(*metaOpt));
            hub.addMod(std::move(meta));
            count++;
        }
    }

    LOG_INFO(QString("模组扫描完成，共 %1 个").arg(count));
}

// ==================== 内容扫描 ====================

void ModScanner::scanAllContents()
{
    auto& hub = RegistryHub::instance();

    for (const auto& meta : hub.allMods()) {
        switch (meta.engineType) {
        case ModEngineType::PE: scanPEContent(meta); break;
        case ModEngineType::VS: scanVSContent(meta); break;
        default: break;
        }
    }

    LOG_INFO(QString("内容扫描完成。总计: VS(%1角色 %2歌曲 %3关卡 %4舞台 %5箭头皮肤) | PE(%6角色 %7歌曲 %8周目 %9舞台)")
        .arg(hub.vsCharacters.size()).arg(hub.vsSongs.size()).arg(hub.vsLevels.size())
        .arg(hub.vsStages.size()).arg(hub.vsNoteStyles.size())
        .arg(hub.peCharacters.size()).arg(hub.peSongs.size())
        .arg(hub.peWeeks.size()).arg(hub.peStages.size()));
}

void ModScanner::scanOneMod(const QString& modIdOrPath)
{
    auto& hub = RegistryHub::instance();

    for (const auto& meta : hub.allMods()) {
        if (meta.id == modIdOrPath || meta.modPath == modIdOrPath) {
            switch (meta.engineType) {
            case ModEngineType::PE: scanPEContent(meta); break;
            case ModEngineType::VS: scanVSContent(meta); break;
            default: break;
            }
            return;
        }
    }
    LOG_WARNING(false, "未找到模组: " + modIdOrPath);
}

// ==================== 引擎专用扫描 ====================

void ModScanner::scanPEContent(const ModMetadata& meta)
{
    auto& hub = RegistryHub::instance();
    QString modPath = meta.modPath;

    auto chars = PEModScanner::scanCharacters(modPath);
    for (auto& pair : chars)
        hub.peCharacters[pair.first] = std::move(pair.second);

    auto songs = PEModScanner::scanSongs(modPath);
    for (auto& song : songs)
        hub.peSongs[song.songId] = std::move(song.difficulties);

    auto weeks = PEModScanner::scanWeeks(modPath);
    for (auto& pair : weeks)
        hub.peWeeks[pair.first] = std::move(pair.second);

    auto stages = PEModScanner::scanStages(modPath);
    for (auto& pair : stages)
        hub.peStages[pair.first] = std::move(pair.second);

    LOG_INFO("PE 内容扫描完成: " + meta.id.value_or("?"));
}

void ModScanner::scanVSContent(const ModMetadata& meta)
{
    auto& hub = RegistryHub::instance();
    QString modPath = meta.modPath;

    auto chars = VSModScanner::scanCharacters(modPath);
    for (auto& pair : chars)
        hub.vsCharacters[pair.first] = std::move(pair.second);

    auto songs = VSModScanner::scanSongs(modPath);
    for (auto& pair : songs)
        hub.vsSongs[pair.first] = std::move(pair.second);

    auto levels = VSModScanner::scanLevels(modPath);
    for (auto& pair : levels)
        hub.vsLevels[pair.first] = std::move(pair.second);

    auto stages = VSModScanner::scanStages(modPath);
    for (auto& pair : stages)
        hub.vsStages[pair.first] = std::move(pair.second);

    auto noteStyles = VSModScanner::scanNoteStyles(modPath);
    for (auto& pair : noteStyles)
        hub.vsNoteStyles[pair.first] = std::move(pair.second);

    LOG_INFO("VS 内容扫描完成: " + meta.id.value_or("?") +
             " (角色:" + QString::number(chars.size()) +
             " 歌曲:" + QString::number(songs.size()) +
             " 关卡:" + QString::number(levels.size()) +
             " 舞台:" + QString::number(stages.size()) +
             " 皮肤:" + QString::number(noteStyles.size()) + ")");
}
