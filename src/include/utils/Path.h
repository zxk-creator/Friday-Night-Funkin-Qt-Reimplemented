#pragma once
#include "src/include/audio/SoundEnum.h"
#include <qfileinfo.h>
#include <qqmlintegration.h>

#include "QDir"
#include "modding/ModEngineType.h"

using FunkinPath = QString;

// 对应VS data目录下的文件（参考自VS Sky Redux，可能少了些东西？不知道）
enum class EDataResourceType
{
	character,
	dialogue,
	levels,
	notestyles,
	notesplashes,
	songs,
	stages,
	stickerpacks,
	ui
};

enum class AssetType {
	TEXT,
	SOUND,
	MUSIC,
	IMAGE,
	BINARY,
	NONE
};

/**
  * @brief 全平台适用。Windows若安装在Program Files目录下需要管理员权限，Android若没有授予管理所有文件的权限，mod功能将无法使用。
  */
class Path
{
public:
	static inline QString currentLevel;

	// 我们将使用Qt的文件系统来记录文件地址，先硬编码
	static inline FunkinPath confirmSoundPath;
	static inline FunkinPath scrollSoundPath;
	static inline FunkinPath cancelSoundPath;
	static inline FunkinPath titleThemePath;
	static inline FunkinPath infoSoundPath;

	static inline bool isInitialized = false;

	static FunkinPath getDefaultSoundPath(EDefaultSoundType soundType);

	static void setDefaultSoundPath(const FunkinPath &soundAbsolutePath, EDefaultSoundType soundType);

	/**
	 * @brief 检查并初始化 Mod 根目录
	 */
	static void checkWhetherModDirExists();

	/**
	 * @brief 获取mod所在目录
	 * @return D:/xxx 这样的绝对路径
	 */
	static QString getModDir();

	/**
	 * 获取资产目录，跨平台适用
	 * @return 构造好的绝对路径
	 */
	static QString getAssetRoot();

	/**
	 * 适用于Android游戏启动，一启动就把资产拷贝到软件“私有”目录，因此启动前一定要调用一下这个。自带检查。
	 * @return 是否成功
	 */
	static bool copyAssets();

	/**
	 * @deprecated 我已经不用qrc了，所以这个也没什么用了
	 * @brief 用于将qrc路径以及相对路径到到绝对路径
	 * @param filePath 绝对路径，相对路径
	 * @return 真 绝对路径。
	 */
	static QString finalModPath(const QString& filePath);

	/**
	 * @brief 获取mods目录下的所有子目录路径并返回一个数组
	**/
	static QVector<QString> getAllModFolderPaths();

	/**
	 * 获取官方引擎data目录下json所在路径，返回值比如（省掉了前面的内容）"coolmod/data/characters")
	 * @param type 资源类型
	 * @param modAbsolutePath 模组根目录绝对路径
	 * @return 构造好的路径，如果您传入了错误的数据，会导致异常，程序终止防止你写错。
	 */
	static QString getVSDataPath(EDataResourceType type, const QString& modAbsolutePath);

	/**
	 * 获取PE模组里面的json文件所在路径
	 * @param type 资产类型
	 * @param modAbsolutePath 模组根目录
	 * @return 找到的绝对路径(由于shared目录存在，因此返回数组，需要遍历执行)
	 */
	static QVector<QString> getPEDataPath(EDataResourceType type, const QString& modAbsolutePath);

	/**
	 * 获取ogg或mp3文件所在位置
	 * @param modAbsolutePath 模组根目录
	 * @param songId 歌曲id
	 * @return mp3或ogg所在目录（不包括文件自身）
	 */
	static QString getVSSongFilePath(const QString& modAbsolutePath,const QString& songId)
	{
		return modAbsolutePath + QDir::separator() + "songs" + QDir::separator() + songId;
	}

private:
	// 下面是我们模仿原版写的方法！用于HScript调用
	static void setCurrentLevel(const QString& level)
	{
		currentLevel = level;
	}

	// 获得内置或者mod资源
	static QString getPreloadPath(const QString& file)
	{
		// 'assets/$file'
		return "assets/" + file;
	}

	// 获得内置或者mod资源
	static QString getLibraryPathForce(const QString& file, const QString& library) {
		// '$library:assets/$library/$file'
		return library + ":assets/" + library + "/" + file;
	}

	static bool tryFileInPaths(const QStringList& paths, const QString& relativePath,
						   const QString& type, QString& outPath);

	static QStringList getExtensionsForType(const QString& type);

	// 为了兼容HScript，我选择使用字符串传参而不是枚举

	static QString resolveAssetPath(const QString& path, const QString& type);

public:
	/**
	 * 辅助方法，外部应该调用这个来获取”绝对路径“
	 * ！！注意！！！！ 不需要加文件拓展名！！加了反而出问题！！
	 * @param file 文件名或相对路径
	 * @param type 会根据他进行类型匹配
	 * @param library :前面的库名，如shared，{modId}(解析json的时候必加，否则找不到！)，不加自动查找所有位置
	 * @return 构造好的绝对路径
	 */
	static QString file(const QString& file, const QString& type, const QString& library, ModEngineType modType);

	static QString image(const QString& key, ModEngineType modType, const QString& library = "") { return file( "images/" + key, "IMAGE", library,modType); }
	static QString sound(const QString& key, ModEngineType modType, const QString& library = "") { return file("sounds/" + key, "SOUND", library,modType); }
	static QString music(const QString& key, ModEngineType modType, const QString& library = "") { return file("music/" + key, "MUSIC", library,modType); }
	static QString json(const QString& key, ModEngineType modType, const QString& library = "") { return file(key, "TEXT", library,modType) + ".json"; }
	static QString font(const QString& key, ModEngineType modType, const QString& library = "") { return file("fonts/" + key, "TEXT", library,modType);}
};

// 辅助类：给QML调用用的，还必须遵守人家的单例模式
class PathUtil : public QObject
{
	Q_OBJECT

public:
	PathUtil() = default;

	Q_INVOKABLE QString font(const QString& name)
	{
		return "file:///" + Path::font(name,ModEngineType::VS);
	}

	Q_INVOKABLE QString image(const QString& name)
	{
		return "file:///" + Path::image(name,ModEngineType::VS);
	}
};

