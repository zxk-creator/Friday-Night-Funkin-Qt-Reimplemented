#include <qicon.h>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QGuiApplication>
#include <gameloop/GameLoop.h>
#include <audio/FunkinSound.h>
#include <utils/InstanceLibrary.h>

#ifdef Q_OS_ANDROID
#include "permissions/GetAndroidStoragePermission.h"
#endif

// 暴露C++函数给QML调用
void initQmlEngine(QQmlApplicationEngine &engine)
{
    engine.rootContext()->setContextProperty("SoundSystem",InstanceLibrary::funkinSoundSystem);
    engine.rootContext()->setContextProperty("ModManager",InstanceLibrary::modManager);
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
#if defined(Q_OS_WIN) && QT_VERSION_CHECK(5, 6, 0) <= QT_VERSION && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    // 窗口图标
    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/qt/qml/fnf/BF.ico"));

    // 初始化子系统
    InstanceLibrary::initSubSystems();
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