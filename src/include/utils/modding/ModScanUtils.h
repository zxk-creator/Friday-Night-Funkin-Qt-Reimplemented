//
// Created by kkplay on 4/16/26.
//

#pragma once
#include "data/mod/ModMetadata.h"

class ModRegistry;

using std::nullopt;

class ModScanUtils
{
public:
    static ModEngineType judgeModEngine(const QString& modAbsolutePath);

    /**
     * 注意，这是扫描单个模组的元数据文件，真的启动扫描的方法在ModManager里面，调用后，会自动将元数据注册到注册表中。
     * @param modAbsolutePath 模组内容所在的根目录
     */
    static void scanPEModMetadata(const QString& modAbsolutePath,ModRegistry* registryRef);
    /**
     * 参数和作用同上。
     */
    static void scanVSModMetadata(const QString& modAbsolutePath,ModRegistry* registryRef);

    /** TODO:先扔这儿了 **/
    static void scanKEModMetadata(const QString& modAbsolutePath) {}
    static void scanCNEModMetadata(const QString& modAbsolutePath) {}

    /**
     *  真正扫描一个模组文件夹里面的东西！您只需要调用这个即可
     *  支持PE和VS，自动加载！
     *  @param modMetadatas 要扫描的模组的元数据
     *  没有返回值，自动注册到注册表中，开始玩了才会加载
     */
    static void scanAllMods(const QVector<ModMetadata>& modMetadatas);

    /**
     * 解析单个模组，适用于查看模组详情界面
     * @param modMetadata 要扫描的模组元数据
     */
    static void scanOneMod(const ModMetadata& modMetadata);

private:
    /**
     * 我认为函数不应该返回任何值导致相互依赖，应该每个函数都到对应的位置自取
     * 我们这里采用懒加载模式，解析后的数据只是一堆路径字符串，并非真的加载到内存中
     * PE和V-Slice都适用！判断逻辑写到这里面了
     * @return 模组解析是否成功。
     * TODO: CNE在HScript解释器写出来之前，估计这里是不会给他适配了
     */
    static bool parseWeeks(const ModMetadata& modMetadata);
    static bool parseSongs(const ModMetadata& modMetadata);
    static bool parseCharacters(const ModMetadata& modMetadata);
    static bool parseStages(const ModMetadata& modMetadata);
    static bool parseNotestyles(const ModMetadata& modMetadata);
    // TODO:dialogue，ui（官方引擎freeplay里面的），stickerpack（官方引擎）暂时用不到，所以不写了！
};
