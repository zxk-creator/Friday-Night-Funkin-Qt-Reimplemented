//
// Created by 44224 on 3/29/2026.
//

#include "src/include/utils/Path.h"

#include <qcoreapplication.h>
#include <qstandardpaths.h>

#include "src/include/ui/buttons/FButton.h"

FunkinPath Path::getDefaultSoundPath(EDefaultSoundType soundType) {
    switch (soundType)
    {
        case EDefaultSoundType::confirm:
            return confirmSoundPath;
        case EDefaultSoundType::scroll:
            return scrollSoundPath;
        case EDefaultSoundType::back:
            return cancelSoundPath;
        default:
            return confirmSoundPath;
    }
}

void Path::setDefaultSoundPath(const FunkinPath &soundAbsolutePath, EDefaultSoundType soundType) {
    if (!QFileInfo::exists(soundAbsolutePath))
        return;
    switch (soundType)
    {
        case EDefaultSoundType::confirm:
            confirmSoundPath = soundAbsolutePath; break;
        case EDefaultSoundType::scroll:
            scrollSoundPath = soundAbsolutePath; break;
        case EDefaultSoundType::back:
            cancelSoundPath = soundAbsolutePath; break;
        default:
            break;
    }
}

void Path::checkWhetherModDirExists() {
    QString baseDir;

#ifdef Q_OS_ANDROID
    // Android: /storage/emulated/0/FNFMods
    baseDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/FNFMods";
#else
    // Windows: exe所在目录/mods
    baseDir = QDir(QCoreApplication::applicationDirPath()).filePath("mods");
#endif

    QDir dir(baseDir);
    if (!dir.exists()) {
        qInfo() << "检测到Mod目录不存在，正在尝试创建:" << baseDir;
        // 递归创建所有缺失的父目录
        if (dir.mkpath(".")) {
            qInfo() << "Mod目录创建成功。";
        } else {
            qCritical() << "Mod目录创建失败！请检查磁盘权限。";
        }
    }

    qInfo() << "Mod目录已就绪:" << baseDir;
}

QString Path::getModDir(const QString& modRelativePath) {
    QString baseDir;
#ifdef Q_OS_ANDROID
    baseDir = QStandardPaths::QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/FNFMods";
#else
    baseDir = QCoreApplication::applicationDirPath() + "/mods";
#endif
    QDir finalDir(baseDir);
    QString absolutePath = finalDir.absoluteFilePath(modRelativePath);

    return QDir::toNativeSeparators(absolutePath);
}

QString Path::finalPath(const QString &filePath) {
    QString finalPath;
    if (filePath.isEmpty()) {
        qWarning() << "警告：传入了空路径！";
        return "";
    }
    // qrc路径，这个稍微复杂一点，因为得先把数据拷贝到磁盘上才能被miniaudio使用
    if (filePath.startsWith("qrc:/") || filePath.startsWith(":/")) {
        QString tempPath = QDir::tempPath() + QDir::separator() + QFileInfo(filePath).fileName();
        if (!QFile::exists(tempPath)) {
            QFile::copy(filePath, tempPath);
            QFile::setPermissions(tempPath, QFileDevice::ReadOwner | QFileDevice::ReadUser);
        }
        finalPath = QDir::toNativeSeparators(tempPath);
    }
    // 绝对路径直接返回
    else {
        finalPath = QDir::toNativeSeparators(filePath);
    }

    return finalPath;
}
