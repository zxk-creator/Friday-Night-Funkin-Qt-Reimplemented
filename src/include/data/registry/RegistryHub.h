#pragma once

/**
 * RegistryHub — 全类型注册表中枢
 *
 * 核心思想：VS 和 PE 数据类型完全独立，各自存入独立的 map。
 * 不再尝试在解析时做格式转换。
 *
 * 注意：QHash 不支持 unique_ptr（COW 机制会拷贝），
 * 因此含 unique_ptr 的容器使用 std::unordered_map。
 */

#include <QHash>
#include <QVector>
#include <unordered_map>
#include <memory>

#include "data/mod/ModMetadata.h"
#include "data/character/CharacterData.h"
#include "data/level/LevelData.h"
#include "data/stage/StageData.h"
#include "data/notestyle/NoteStyleData.h"
#include "play/song/Song.h"
#include "data/pe/PECharacterData.h"
#include "data/pe/PESongData.h"
#include "data/pe/PEStageData.h"

class RegistryHub
{
public:
    static RegistryHub& instance()
    {
        static RegistryHub inst;
        return inst;
    }

    // ==================== 模组元数据 ====================
    QVector<ModMetadata> modMetadatas;

    // ==================== VS 注册表 ====================
    // 使用 std::unordered_map 因为 unique_ptr 不可拷贝
    std::unordered_map<QString, std::unique_ptr<CharacterData>> vsCharacters;
    std::unordered_map<QString, std::unique_ptr<Song>> vsSongs;
    std::unordered_map<QString, std::unique_ptr<LevelData>> vsLevels;
    std::unordered_map<QString, std::unique_ptr<StageData>> vsStages;
    std::unordered_map<QString, std::unique_ptr<NoteStyleData>> vsNoteStyles;

    // ==================== PE 注册表 ====================
    std::unordered_map<QString, std::unique_ptr<PECharacterData>> peCharacters;
    // 注：PESongDifficultyData 不包含 unique_ptr，所以 QHash 可用
    QHash<QString, QVector<PESongDifficultyData>> peSongs;
    QHash<QString, PEWeekData> peWeeks;
    QHash<QString, PEStageData> peStages;

    // ==================== 模组管理 ====================

    void addMod(std::unique_ptr<ModMetadata> meta)
    {
        if (meta) modMetadatas.push_back(std::move(*meta));
    }

    const QVector<ModMetadata>& allMods() const { return modMetadatas; }

    int modCount() const { return modMetadatas.size(); }
    int totalEntryCount() const {
        return vsCharacters.size() + vsSongs.size() + vsLevels.size()
             + vsStages.size() + vsNoteStyles.size()
             + peCharacters.size() + peSongs.size() + peWeeks.size() + peStages.size();
    }

    void clearAll()
    {
        modMetadatas.clear();
        vsCharacters.clear();      vsSongs.clear();
        vsLevels.clear();          vsStages.clear();    vsNoteStyles.clear();
        peCharacters.clear();      peSongs.clear();
        peWeeks.clear();           peStages.clear();
    }

private:
    RegistryHub() = default;
    ~RegistryHub() = default;
    RegistryHub(const RegistryHub&) = delete;
    RegistryHub& operator=(const RegistryHub&) = delete;
};
