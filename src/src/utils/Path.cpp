//
// Created by 44224 on 3/29/2026.
//

#include "src/include/utils/Path.h"
#include "utils/exception/CustomException.h"

#include <qcoreapplication.h>
#include <qstandardpaths.h>
#include "utils/file/FileUtil.h"
#include <QDirIterator>

#include "utils/message/MessageHandler.h"

FunkinPath Path::getDefaultSoundPath(EDefaultSoundType soundType) {

    if (!isInitialized)
    {
        isInitialized = true;
        confirmSoundPath = PathVS::sound("confirmMenu").value();
        scrollSoundPath = PathVS::sound("scrollMenu").value();
        cancelSoundPath = PathVS::sound("cancelMenu").value();
        infoSoundPath = PathVS::sound("boop").value();
        titleThemePath = PathVS::file("preload/music/freakyMenu/freakyMenu","MUSIC","").value();
    }

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
    // Windows: exe所在目录/assets
    baseDir = QDir(QCoreApplication::applicationDirPath()).filePath("assets");
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
        finalDir.mkpath(".");
        qInfo() << "mod目录不存在！正在创建....";
    }
    MessageHandler::logInfo("mod目录: " + baseDir,"Path::getModDir");

    return QDir::toNativeSeparators(baseDir);
}

QString Path::getAssetRoot()
{
#ifdef Q_OS_ANDROID
    // 安卓：sdcard/Android/data/com.kunkun.fnfqe/files/assets
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/assets";
#else
    // 其他: 可执行所在目录/assets
    return QCoreApplication::applicationDirPath() + QDir::separator() + "assets";
#endif
}

bool Path::copyAssets()
{
#ifdef Q_OS_ANDROID
    QString privateDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString assetRoot = privateDir + "/assets";
    // 标志: 文件是否是我拷贝过的？
    QString flagFile = privateDir + "/.assets_copied";
    if (QFile::exists(flagFile)){
        MessageHandler::logWarning(QString("资产目录已存在！将不会被拷贝。"),QString("Path"));
        return false;
    }
    // 说明没有看来是，我们先清理目录
    QDir(assetRoot).removeRecursively();
    if (!QDir().mkpath(assetRoot)) return false;

    MessageHandler::logInfo(QString("准备开始从APK复制到data私有目录..."),QString("Path"));
    QStringList filesToCopy;
    // 从APK中的assets目录进行复制
    QDirIterator it("assets:/", QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        if (it.fileInfo().isFile()) {
            filesToCopy.append(it.filePath());
        }
    }

    MessageHandler::logInfo(QString("找到了" + QString::number(filesToCopy.size()) + "文件在APK中！"),"Path");

    for (const QString& source : filesToCopy) {
        QString relative = source.mid(QString("assets:/").length());
        QString dest = assetRoot + "/" + relative;

        QDir().mkpath(QFileInfo(dest).absolutePath());

        if (!QFile::copy(source, dest)) {
            MessageHandler::logWarning(QString("拷贝失败，你的磁盘空间可能不足？文件:" + source),"Path");
        }
    }

    QFile flag(flagFile);
    if (flag.open(QIODevice::WriteOnly)){
        flag.write("1");
        flag.close();
    }

    MessageHandler::logInfo(QString("文件从APK拷贝到私有目录完成!"),"Path");

    return true;
#else
    MessageHandler::logWarning(true, QString("此函数只适用于Android。"),"Path");
    return false;
#endif
}

QString Path::finalModPath(const QString &filePath)
{
    if (filePath.isEmpty()) {
        qWarning() << "[Path] 警告：传入了空路径！";
        return {};
    }

    qInfo() << "[Path] 原始路径：" << filePath;

    QString resultPath;
    QFileInfo checkFile(filePath);

    if (checkFile.isAbsolute()) {
        // 已经是绝对路径，直接使用
        resultPath = filePath;
    }
    else {
        // 相对路径基于当前mod目录拼接
        QDir base(getModDir());
        resultPath = base.absoluteFilePath(filePath);
    }

    // 规范化路径
    resultPath = QDir::cleanPath(resultPath);
    QString finalPath = QDir::toNativeSeparators(resultPath);

    qInfo() << "[Path] 最终绝对路径：" << finalPath;
    return finalPath;
}

QVector<QString> Path::getModSubFolderPaths()
{
    QString modFolderPath = getModDir();
    QVector<QString> subDirs;
    QDir dir(modFolderPath);

    if (!dir.exists()){
        // 说明逻辑都写错了
        MessageHandler::logWarning(true,"警告: " + modFolderPath + "不存在！","Path");
        std::terminate();
    }

    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

    QFileInfoList infoList = dir.entryInfoList();

    for (const QFileInfo& info : infoList) {
        subDirs.push_back(info.absoluteFilePath());
    }

    return subDirs;
}

QStringList Path::getSubDirs(const QString& parentFolderPath)
{
    QStringList subDirs;
    QDir dir(parentFolderPath);

    if (!dir.exists()) return subDirs;

    const QFileInfoList infoList = dir.entryInfoList(
        QDir::Dirs | QDir::NoDotAndDotDot,
        QDir::Name
    );

    for (const QFileInfo& info : infoList) {
        subDirs.append(info.absoluteFilePath());
    }

    return subDirs;
}

QString PathVS::getDataPath(EDataResourceType type, const QString& modAbsolutePath)
{
    QString targetDirName;
    switch (type)
    {
    case EDataResourceType::character:
        {
            targetDirName = "characters";
            break;
        }
    case EDataResourceType::dialogue:
        {
            targetDirName = "dialogue";
            break;
        }
    case EDataResourceType::levels:
        {
            targetDirName = "levels";
            break;
        }
    case EDataResourceType::notestyles:
        {
            targetDirName = "notestyles";
            break;
        }
    case EDataResourceType::songs:
        {
            targetDirName = "songs";
            break;
        }
    case EDataResourceType::stages:
        {
            targetDirName = "stages";
            break;
        }
    case EDataResourceType::stickerpacks:
        {
            targetDirName = "stickerpacks";
            break;
        }
    case EDataResourceType::ui:
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

QStringList PathVS::getExtensionsForType(const QString& type) {
    if (type == "IMAGE") return {".png", ".jpg", ".jpeg"};
    if (type == "SOUND" || type == "MUSIC") return {".ogg", ".mp3", ".wav"};
    if (type == "TEXT") return {".json", ".xml", ".txt",".ttf", ".frag", ".vert"};
    if (type == "BINARY") return {".bin", ".dat", ".mp4"};
    return {""};
}

bool PathVS::tryFileInPaths(const QStringList& paths, const QString& relativePath,
                           const QString& type, QString& outPath) {
    QStringList extensions = getExtensionsForType(type);

    for (const QString& basePath : paths) {
        for (const QString& ext : extensions) {
            QString fullPath = basePath + QDir::separator() + relativePath + ext;
            QString normalized = QDir::cleanPath(fullPath);

            if (QFile::exists(normalized)) {
                outPath = normalized;
                return true;
            }
        }
    }
    return false;
}

QString PathVS::resolveAssetPath(const QString& path, const QString& type) {
    // 他可能是 "library:assets/library/..." 或 "assets/..."等等等等）
    QString searchPath = path;

    int colonPos = path.indexOf(':');
    if (colonPos != -1) {
        searchPath = path.mid(colonPos + 1);
    }
    // 去掉开头的"assets/"
    if (searchPath.startsWith("assets/")) {
        searchPath = searchPath.mid(7);
    }

    // 构建查找路径列表（按优先级从高到低）
    QStringList searchPaths;

    // 1. 模组目录
    QString modDir = Path::getModDir();
    QDir modsDir(modDir);
    if (modsDir.exists()) {
        for (const QString& modId : modsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            searchPaths << modDir + "/" + modId + "/assets";
        }
    }

    // 2. 当前关卡目录
    if (!Path::currentLevel.isEmpty()) {
        searchPaths << Path::getAssetRoot() + "/" + Path::currentLevel;
    }

    // 3. shared目录
    searchPaths << Path::getAssetRoot() + "/shared";

    // 4. 主assets目录
    searchPaths << Path::getAssetRoot();

    // 5. 可执行文件目录作为备选方案
    searchPaths << QCoreApplication::applicationDirPath();

    QString outPath;
    if (tryFileInPaths(searchPaths, searchPath, type, outPath)) {
        return outPath;
    }

    return "";
}

std::optional<QString> PathVS::file(const QString& file, const QString& type, const QString& library) {
    // 1. 解析库前缀，确定要查找的根路径
    QString searchPath;
    QString actualLibrary = library;

    // 如果file本身包含库前缀，优先使用
    int colonPos = file.indexOf(':');
    if (colonPos != -1 && library.isEmpty()) {
        actualLibrary = file.left(colonPos);
        searchPath = file.mid(colonPos + 1);
        if (searchPath.startsWith("assets/")) {
            searchPath = searchPath.mid(7);
        }
    } else {
        searchPath = file;
    }

    // 2. 根据库名构建查找路径
    QStringList searchPaths;

    if (!actualLibrary.isEmpty()) {
        // 指定了库名
        if (actualLibrary == "preload" || actualLibrary == "default") {
            // preload/default → 主 assets 目录
            searchPaths << Path::getAssetRoot();
        }
        else if (actualLibrary == "shared") {
            // shared → assets/shared/
            searchPaths << Path::getAssetRoot() + "/shared";
        }
        else if (actualLibrary == "songs") {
            // songs → assets/songs/
            searchPaths << Path::getAssetRoot() + "/songs";
        }
        else {
            // 模组库 → mods/{modId}/assets/
            searchPaths << Path::getModDir() + QDir::separator() + actualLibrary + QDir::separator() +"assets";
        }
    }
    else {
        // 无库名自动查找
        // 1) 模组目录
        QString modDir = Path::getModDir();
        QDir modsDir(modDir);
        if (modsDir.exists()) {
            for (const QString& modId : modsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
                searchPaths << modDir + QDir::separator() + modId + QDir::separator() + "assets";
            }
        }

        // 2) 当前关卡目录
        if (!Path::currentLevel.isEmpty()) {
            searchPaths << Path::getAssetRoot() + QDir::separator() + Path::currentLevel;
        }

        // 3) shared目录
        searchPaths << Path::getAssetRoot() + QDir::separator() + "shared";

        // 4) assets目录
        searchPaths << Path::getAssetRoot();
    }

    // 3. 尝试查找文件
    QString outPath;
    if (tryFileInPaths(searchPaths, searchPath, type, outPath)) {
        return outPath;
    }

    // 4. 未找到
    QString extensions = getExtensionsForType(type).join(", ");
    MessageHandler::logWarning(false,
    QString("找不到资产: %1 类型: %2 库名为: %3").arg(file).arg(type).arg(library),
    "PathVS::file");
    return "";
}

QVector<QString> PathPE::getDataPath(EDataResourceType type, const QString& modAbsolutePath)
{
    QString sharedDir = "/shared";
    QString targetDirName;
    switch (type)
    {
    case EDataResourceType::character:
        {
            targetDirName = "/characters";
            break;
        }
    case EDataResourceType::dialogue:
        {
            // TODO: 暂时搁置，不支持自定义对话框（目前甚至不会弹出）
            targetDirName = "/dialogue";
            break;
        }
    case EDataResourceType::levels:
        {
            targetDirName = "/weeks";
            break;
        }
    case EDataResourceType::notestyles:
        {
            targetDirName = "/images/noteSkins";
            break;
        }
    case EDataResourceType::songs:
        {
            targetDirName = "/data";
            break;
        }
    case EDataResourceType::stages:
        {
            targetDirName = "/stages";
            break;
        }
    case EDataResourceType::notesplashes:
        {
            targetDirName = "/images/noteSplashes";
            break;
        }
    default:
        {
            LOG_WRONG_PARAM_ERROR("你写错了参数！");
            break;
        }
    }

    QString sharedRes = QDir::cleanPath(modAbsolutePath + sharedDir + targetDirName);
    QString normalRes = QDir::cleanPath(modAbsolutePath + targetDirName);

    return {sharedRes,normalRes};
}

std::optional<QString> PathPE::file(const QString& modAbsolutePath, const QString& file, const QString& type)
{
    QString normalPath = modAbsolutePath;
    QString sharedPath = modAbsolutePath + "/shared/";
    // 图片，直接后面加png（只支持png）
    if (type == "IMAGE")
    {
        normalPath += "/images/" + file + ".png";
        sharedPath += "/images/" + file + ".png";
        if (QFile::exists(QDir::cleanPath(sharedPath)))
            return sharedPath;
        if (QFile::exists(normalPath))
            return normalPath;
        // 走到这说明确实没这个文件
        LOG_NO_SUCH_FILE_ERROR(normalPath);
        return std::nullopt;
    }
    if (type == "JSON")
    {
        // json有很多，需要去指定路径下找并返回
        LOG_ERROR(true, "暂不支持json，程序即将终止...");
        std::terminate();
    }
    else
    {
        LOG_WARNING(false,QString("暂不支持的文件类型") + file);
        return std::nullopt;
    }
}

QString PathUtil::getPathLeaf(const QString& path)
{
    std::optional<QString> res = FileUtil::getPathLeaf(path);
    if (!res.has_value())
    {
        LOG_WARNING(false,"QML尝试获取模组路径" + path + "的leaf的时候出现问题！");
        return "";
    }

    return res.value();
}
