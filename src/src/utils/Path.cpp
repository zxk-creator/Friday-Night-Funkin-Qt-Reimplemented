//
// Created by 44224 on 3/29/2026.
//

#include "src/include/utils/Path.h"

#include <qcoreapplication.h>
#include <qstandardpaths.h>
#include <QDirIterator>

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
    baseDir = "/sdcard/FNFMods";
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

QString Path::getModDir() {
    QString baseDir;
    // 安卓系统上，模组目录位于/stroage/emuilated/0/FNFMods
#ifdef Q_OS_ANDROID
    baseDir = "/sdcard/FNFMods";
#else
    // 其余的在可执行文件所在目录，名为mods
    baseDir = QCoreApplication::applicationDirPath() + "/mods";
#endif
    QDir finalDir(baseDir);
    if (!finalDir.exists()) {
        finalDir.mkpath("."); // 确保 baseDir 存在
        qInfo() << "mod目录不存在！正在创建....";
    }
    qInfo() << "mod目录："<< baseDir;

    return QDir::toNativeSeparators(baseDir);
}

QString Path::finalModPath(const QString &filePath) {
    if (filePath.isEmpty()) {
        qWarning() << "警告：传入了空路径！";
        return {};
    }

    QString resultPath;
    qInfo() << "原始路径：" << filePath;

    // miniaudio等外部C库无法直接读取内存中的qrc，必须释放到磁盘
    if (filePath.startsWith(":/") || filePath.startsWith("qrc:/")) {
        QString cleanPath = filePath.startsWith("qrc:/") ? filePath.mid(3) : filePath;
        QFileInfo qrcInfo(cleanPath);
        QString tempPath = QDir::tempPath() + "/FunkinCache/" + qrcInfo.fileName();

        QDir().mkpath(QFileInfo(tempPath).absolutePath());

        // 强制覆盖旧缓存，防止文件损坏或已存在
        if (QFile::exists(tempPath)) {
            QFile::remove(tempPath);
        }

        if (!QFile::copy(cleanPath, tempPath)) {
            // 打印具体错误原因
            qCritical() << "无法将资源文件" << cleanPath << "拷贝到" << tempPath
                        << " 原因：" << QFile(cleanPath).errorString();
            return {};
        }

        // 赋予读写权限
        QFile::setPermissions(tempPath, QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ReadUser);
        resultPath = tempPath;
    }
    else {
        // 普通路径(包括绝对与相对路径)
        QFileInfo checkFile(filePath);
        if (checkFile.isRelative()) {
            QDir base(getModDir());
            resultPath = base.absoluteFilePath(filePath);
        } else {
            resultPath = filePath;
        }
    }

    // 规范化路径并转为原生分隔符
    resultPath = QDir::cleanPath(resultPath);
    QString finalPath = QDir::toNativeSeparators(resultPath);

    qInfo() << "最终绝对路径：" << finalPath;
    return finalPath;
}

QVector<QString> Path::getAllModFolderPaths()
{
    QString baseDir = getModDir();
    QVector<QString> res;
    // 过滤掉父目录本身和，递归搜索所有子目录
    QDirIterator it(baseDir,QDir::Dirs | QDir::NoDotAndDotDot,QDirIterator::NoIteratorFlags);
    while (it.hasNext())
    {
        // 返回的就算绝对路径
        res.append(it.next());
    }

    return res;
}

QString Path::getVSDataPath(ResourceType type, const QString& modAbsolutePath)
{
    QString targetDirName;
    switch (type)
    {
    case ResourceType::character:
        {
            targetDirName = "characters";
            break;
        }
    case ResourceType::dialogue:
        {
            targetDirName = "dialogue";
            break;
        }
    case ResourceType::levels:
        {
            targetDirName = "levels";
            break;
        }
    case ResourceType::notestyles:
        {
            targetDirName = "notestyles";
            break;
        }
    case ResourceType::songs:
        {
            targetDirName = "songs";
            break;
        }
    case ResourceType::stages:
        {
            targetDirName = "stages";
            break;
        }
    case ResourceType::stickerpacks:
        {
            targetDirName = "stickerpacks";
            break;
        }
    case ResourceType::ui:
        {
            targetDirName = "ui";
            break;
        }
    default:
        {
            LOG_WRONG_PARAM_ERROR("你写错了参数！");
            break;
        }
    }

    return QDir::cleanPath(modAbsolutePath + QDir::separator() + "data" + QDir::separator() + targetDirName);
}
