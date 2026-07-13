#include <qicon.h>
#include <QQmlContext>
#include <QGuiApplication>
#include <audio/FunkinSound.h>

#include "audio/FunkinSoundSystem.h"
#include "data/Context.h"
#include "QImageReader"
#include "data/mod/ModRegistry.h"
#include "utils/interseting/InterestingThings.h"
#include "utils/Path.h"

#ifdef Q_OS_ANDROID
#include "permissions/GetAndroidStoragePermission.h"
#endif

// 暴露C++函数给QML调用
inline void init(QQmlEngine &engine)
{
    // 初始化存储单例的中心
    auto context = new Context();
    // 虽然确实内存泄露了，但是他毕竟是全局存在！只需要初始化一次

    engine.rootContext()->setContextProperty("SoundSystem",Context::soundSystem);
    engine.rootContext()->setContextProperty("ModRegistry",Context::modRegistry);
    // 扫描模组
    Context::modRegistry->scanAllModMetadatas();
    engine.rootContext()->setContextProperty("PathUtil",Context::pathUtil);
    engine.rootContext()->setContextProperty("log",Context::logProxy);
    engine.rootContext()->setContextProperty("File",Context::fileutil);
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


int main(int argc, char *argv[])
{
    // 去掉注释测试ASan内存分析工具是否正常工作
    /*
    int* p = nullptr;
    *p = 42;
    */
    // printf("Hello World!");

#if defined(Q_OS_WIN) && QT_VERSION_CHECK(5, 6, 0) <= QT_VERSION && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

#ifdef QT_DEBUG
    LOG_INFO("您正处于调试模式下！");
#else
    LOG_INFO("您正处于发布模式下，版本" + Common::CURRENT_ENGINE_VERSION);
#endif

    // 窗口图标
    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/qt/qml/fnf/BF.ico"));

    auto gameWindow = new GameWindow();
    Context::gameWindow = gameWindow;

    #ifdef Q_OS_ANDROID
    requestAndroidPermission();
    Path::copyAssets();
    #endif

    QImageReader::setAllocationLimit(512);

    // 用 GameWindow(QQuickView) 加载 QML
    gameWindow->setTitle("Mod Loader for - Friday Night Funkin'");
    gameWindow->setMinimumSize(QSize(800, 450));
    gameWindow->setResizeMode(QQuickView::SizeRootObjectToView);
    QColor color;
    gameWindow->setColor(color.black());

    // 先设置 context 属性，再加载 QML
    init(*gameWindow->engine());
    gameWindow->setSource(QUrl("qrc:/qt/qml/fnf/QML/main.qml"));

    InterestingThings::damn(false);

    // 测试：放首歌听听
    auto test = new FunkinSound(false,PathVS::file("music/freakyMenu/freakyMenu","MUSIC","").value(),ESoundType::uiSound,true,"freakyMenu");
    test->playSound();

    gameWindow->show();

    return app.exec();
}