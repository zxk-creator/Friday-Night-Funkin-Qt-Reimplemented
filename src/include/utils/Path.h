#pragma once
#include "src/include/audio/SoundEnum.h"
// 获取路径用
#include <QFile>
#include <qfileinfo.h>
#include "QDir"
#include "exception/CustomException.h"

using FunkinPath = QString;

// 对应VS data目录下的文件（参考自VS Sky Redux，可能少了些东西？不知道）
enum class ResourceType
{
	character,
	dialogue,
	levels,
	notestyles,
	songs,
	stages,
	stickerpacks,
	ui
};

/**
  * @brief 全平台适用。Windows若安装在Program Files目录下需要管理员权限，Android若没有授予管理所有文件的权限，mod功能将无法使用。
  */
class Path
{
public:
	// 我们将使用Qt的文件系统来记录文件地址，先硬编码
	static inline FunkinPath confirmSoundPath = ":/mods/default/sounds/confirmMenu.ogg";
	static inline FunkinPath scrollSoundPath = ":/mods/default/sounds/scrollMenu.ogg";
	static inline FunkinPath cancelSoundPath = ":/mods/default/sounds/cancelMenu.ogg";
	static inline FunkinPath titleThemePath = ":/mods/default/sounds/titleTheme.ogg";
	static inline FunkinPath infoSoundPath = ":/mods/default/sounds/boop.wav";

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
	 * 获取数据目录下的一些内容
	 * @param type 资源类型
	 * @param modAbsolutePath 模组根目录绝对路径
	 * @return 构造好的路径，如果您传入了错误的数据，会导致异常，程序终止防止你写错。
	 */
	static QString getVSDataPath(ResourceType type, const QString& modAbsolutePath);

	static 
};

