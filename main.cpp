#include <qicon.h>
#include <QQmlContext>
#include <QGuiApplication>
#include <audio/FunkinSound.h>

#include "audio/FunkinSoundSystem.h"
#include "modding/scan/ModManager.h"

#ifdef Q_OS_ANDROID
#include "permissions/GetAndroidStoragePermission.h"
#endif

// 暴露C++函数给QML调用
inline void initQmlEngine(QQmlApplicationEngine &engine)
{
    engine.rootContext()->setContextProperty("SoundSystem",FunkinSoundSystem::instance());
    engine.rootContext()->setContextProperty("ModManager",ModManager::instance());
    // 以后注册点别的
}

#ifdef Q_OS_ANDROID
void requestAndroidPermission()
{
    if (!hasAllFilesPermission())
        requestAllFilesAccess();
}
#endif

int main(int argc, char *argv[])
{
    // printf("Hello World!");

#if defined(Q_OS_WIN) && QT_VERSION_CHECK(5, 6, 0) <= QT_VERSION && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    // 窗口图标
    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/qt/qml/fnf/BF.ico"));

    #ifdef Q_OS_ANDROID
    // 检查管理所有文件权限是否已get
    requestAndroidPermission();
    #endif

    // 加载qml
    QQmlApplicationEngine engine;
    initQmlEngine(engine);
    engine.load(QUrl(QStringLiteral("qrc:/qt/qml/fnf/QML/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;
    // 测试：放首歌听听
    auto test = new FunkinSound(false,":/mods/default/preload/music/freakyMenu/freakyMenu.ogg",ESoundType::uiSound,true,"freakyMenu");
    test->playSound();

    // 事件循环，防止执行到这窗口就没了
    return app.exec();
}