#include <qicon.h>
#include <QQmlContext>
#include <QGuiApplication>
#include <audio/FunkinSound.h>
#include <QMessageBox>

#include "audio/FunkinSoundSystem.h"
#include "data/Context.h"
#include "QImageReader"
#include "data/mod/ModRegistry.h"
#include "utils/interseting/InterestingThings.h"
#include "utils/Path.h"
#include "HaxeParser/ast/Interpreter.h"
#include "play/states/MenuState.h"
#include "HaxeParser/Init.h"
#include "utils/window/WindowUtil.h"

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

// 注册一些类
void registerSomeClass()
{
    Context::interpreter->registerNativeClass("MenuState", {});
}
// 注册一些本地函数
void registerSomeFunction(Interpreter* interpreter)
{

}


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
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/qt/qml/fnf/BF.ico"));

    auto gameWindow = new GameWindow();
    Context::gameWindow = gameWindow;
    gameWindow->init();

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

    auto interpreter = new Interpreter();
    Context::interpreter = interpreter;
    registerSomeClass();

    // 测试：放首歌听听
    auto test = new FunkinSound(false,PathVS::file("music/freakyMenu/freakyMenu","MUSIC","").value(),ESoundType::uiSound,true,"freakyMenu");
    test->playSound();

    gameWindow->show();



    // ============================================ 异常处理区 ===========================================================
    try {
        // 开始执行脚本，脚本开始接管一切。
        try {
            QString mainScriptPath = Path::getAssetRoot() + "/scripts/Main.hx";
            QString source = FileUtil::ReadFileToString(mainScriptPath);
            HxParser::initInterpreter(source, interpreter);
        }
        catch (std::runtime_error& e) {
            auto* pool = LangStringPool::instance();
            WindowUtil::showQuickDialog(QMessageBox::Critical,
                                        pool->scriptRuntimeError(),
                                        pool->scriptRuntimeErrorMsg().arg(e.what()));
        }
    }
    catch (...) {
        auto* pool = LangStringPool::instance();
        WindowUtil::showQuickDialog(QMessageBox::Critical,
                                    pool->fatalError(),
                                    pool->fatalStartupErrorMsg());
        std::abort();
    }

    try {
        return app.exec();
    }
    catch (std::exception& e) {
        auto* pool = LangStringPool::instance();
        WindowUtil::showQuickDialog(QMessageBox::Critical,
                                    pool->fatalError(),
                                    pool->fatalRuntimeErrorMsg());
    }

    return -1;
}