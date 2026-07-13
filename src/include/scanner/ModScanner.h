#pragma once

/**
 * ModScanner — 模组扫描总调度器
 *
 * 职责：
 * 1. 检测模组引擎类型（PE/VS）
 * 2. 扫描元数据
 * 3. 分派给 VSScanner 或 PEScanner 进行内容解析
 * 4. 将解析结果存入对应的注册表
 *
 * 使用方式：
 *   ModScanner::scanAllMetadatas();   // 扫描所有模组元数据
 *   ModScanner::scanAllContents();    // 扫描所有模组内容
 *   ModScanner::scanOneMod(id);       // 扫描单个模组
 */

#include <QString>
#include <QVector>
#include <optional>

#include "data/mod/ModMetadata.h"
#include "modding/ModEngineType.h"

class ModScanner
{
public:
    /**
     * 判断模组引擎类型
     */
    static ModEngineType judgeEngine(const QString& modAbsolutePath);

    /**
     * 扫描所有模组的元数据并注册
     * 遍历 mods 目录，检测引擎类型，解析元数据
     */
    static void scanAllMetadatas();

    /**
     * 扫描所有模组的内容（角色/歌曲/关卡/舞台/箭头皮肤）
     */
    static void scanAllContents();

    /**
     * 扫描单个模组（元数据 + 所有内容）
     * @param modIdOrPath 模组 ID 或绝对路径
     */
    static void scanOneMod(const QString& modIdOrPath);

    /**
     * 扫描单个模组的元数据
     * @param modAbsolutePath 模组绝对路径
     * @return 如果成功返回 ModMetadata，失败返回 nullopt
     */
    static std::optional<ModMetadata> scanOneMetadata(const QString& modAbsolutePath);

private:
    // 引擎专用扫描（被 scanAllContents / scanOneMod 调用）
    static void scanPEContent(const ModMetadata& meta);
    static void scanVSContent(const ModMetadata& meta);
};
