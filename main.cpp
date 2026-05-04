#include <qicon.h>
#include <QQmlContext>
#include <QGuiApplication>
#include <audio/FunkinSound.h>

#include "audio/FunkinSoundSystem.h"
#include "modding/scan/ModManager.h"
#include "utils/interseting/InterestingThings.h"
#include "play/event/IHookEvents.h"
#include "utils/Path.h"

#ifdef Q_OS_ANDROID
#include "permissions/GetAndroidStoragePermission.h"
#endif

// 暴露C++函数给QML调用
inline void initQmlEngine(QQmlApplicationEngine &engine)
{
    engine.rootContext()->setContextProperty("SoundSystem",FunkinSoundSystem::instance());
    engine.rootContext()->setContextProperty("ModManager",ModManager::instance());
    engine.rootContext()->setContextProperty("PathUtil",PathUtil::instance());
    // 以后注册点别的
}

#ifdef Q_OS_ANDROID
// 检查管理所有文件权限是否已get
void requestAndroidPermission()
{
    if (!hasAllFilesPermission())
        requestAllFilesAccess();
}
#endif


/**
 * 我一直在想，注释到底用英文写还是中文写？
 * 毕竟国内FNF热度一般，又有多少人会关注我这个项目
 * 国外也一般半
 * 算了
 * 自娱自乐
 * 注释还是Chinese+
 */
int main(int argc, char *argv[])
{
    // 看看ASan内存分析工具是否正常工作
    /*
    int* p = nullptr;
    *p = 42;
    */
    // printf("Hello World!");

#if defined(Q_OS_WIN) && QT_VERSION_CHECK(5, 6, 0) <= QT_VERSION && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    // 窗口图标
    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/qt/qml/fnf/BF.ico"));

    #ifdef Q_OS_ANDROID
    requestAndroidPermission();
    Path::copyAssets();
    #endif

    // 加载qml
    QQmlApplicationEngine engine;
    initQmlEngine(engine);
    engine.load(QUrl(QStringLiteral("qrc:/qt/qml/fnf/QML/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    InterestingThings::damn(false);

    // 测试：放首歌听听
    auto test = new FunkinSound(false,Path::file("preload/music/freakyMenu/freakyMenu","MUSIC",""),ESoundType::uiSound,true,"freakyMenu");
    test->playSound();

    // 事件循环，防止执行到这窗口就没了
    return app.exec();
}