//
// Created by kkplay on 4/16/26.
//

#pragma once
#include <modding/modmetadata/ModMetadata.h>

using std::nullopt;

class ModScanUtils
{
public:
    static ModEngineType judgeModEngine(const QString& modAbsolutePath);
    static optional<ModMetadata> scanPEModMetadata(QString& modAbsolutePath);
    static optional<ModMetadata> scanVSModMetadata(QString& modAbsolutePath);
    /** TODO:先扔这儿了 **/
    static optional<ModMetadata> scanKEModMetadata(QString& modAbsolutePath) { return nullopt;}
    static optional<ModMetadata> scanCNEModMetadata(QString& modAbsolutePath) { return nullopt;}

    /**
     *  真正扫描一个模组文件夹里面的东西！您只需要调用这个即可
     *  支持PE和VS，自动加载！
     *  @param modAbsolutePath: 传入模组的绝对路径
     *  @param modMetadata: scanModMetadata的返回值
     *  没有返回值，自动注册到注册表中，开始玩了才会加载
     */
    static void scanAllMods(const QVector<ModMetadata>& modMetadatas);

private:
    /**
     * 我认为函数不应该返回任何值导致相互依赖，应该每个函数都到对应的位置自取
     * 我们这里采用懒加载模式，解析后的数据只是一堆路径字符串，并非真的加载到内存中
     * PE和V-Slice都适用！判断逻辑写到这里面了
     * @return 模组解析是否成功。
     * TODO: CNE在HScript解释器写出来之前，估计这里是不会给他适配了
     */
    static bool parseWeeks(const ModMetadata& modMetadata);
    static bool parseCharacters(const ModMetadata& modMetadata);
    static bool parseSongs(const ModMetadata& modMetadata);
    static bool parseStages(const ModMetadata& modMetadata);
    static bool parseNotestyles(const ModMetadata& modMetadata);
    // TODO:dialogue，ui（官方引擎freeplay里面的），stickerpack（官方引擎）暂时用不到，所以不写了！
};
