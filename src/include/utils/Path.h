#pragma once
#include "src/include/audio/SoundEnum.h"
// 获取路径用
#include <QFile>
#include <qfileinfo.h>
#include "QDir"

using FunkinPath = QString;

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
};

