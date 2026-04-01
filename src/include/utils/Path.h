#pragma once
#include "src/include/audio/SoundEnum.h"
// 获取路径用
#include <QFile>
#include <qfileinfo.h>
#include "QDir"

using FunkinPath = QString;
namespace fs = std::filesystem;

/**
  * @brief 全平台适用。Windows若安装在Program Files目录下需要管理员权限，Android若没有授予管理所有文件的权限，mod功能将无法使用。
  */
class Path
{
private:
	// 我们将使用Qt的文件系统来记录文件地址，先硬编码
	static inline FunkinPath confirmSoundPath = ":/mods/default/sounds/confirmMenu.ogg";
	static inline FunkinPath scrollSoundPath = ":/mods/default/sounds/scrollMenu.ogg";
	static inline FunkinPath cancelSoundPath = ":/mods/default/sounds/cancelMenu.ogg";

public:
	static FunkinPath getDefaultSoundPath(EDefaultSoundType soundType);

	static void setDefaultSoundPath(const FunkinPath &soundAbsolutePath, EDefaultSoundType soundType);

	/**
	 * @brief 检查并初始化 Mod 根目录
	 */
	static void checkWhetherModDirExists();

	/**
	 * @brief 传入相对路径如xxx/xxx，返回绝对路径，自动适配移动端和Windows（无需加mods前缀），安卓端必须获取管理所有文件的权限！！！否则模组功能将无法使用！！！
	 * @param relativePath Vs_xxx/data 类似的相对路径
	 * @return D:/xxx 绝对路径
	 */
	static QString getModDir(const QString& modRelativePath);

	/**
	 * @brief 用于转换qrc文件路径到绝对路径
	 * @param filePath 绝对路径，不支持相对路径，请先调用getModsDir获取模组目录，再拼接，qrc路径，都可以
	 * @return 真 绝对路径。
	 */
	static QString finalPath(const QString& filePath);
};

