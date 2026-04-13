#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qicon.h>
#include <QQmlContext>
#include <qmlReloader/QmlReloader.h>
#include <gameloop/GameLoop.h>
#include <audio/FunkinSound.h>
#include <utils/InstanceLibrary.h>

int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN) && QT_VERSION_CHECK(5, 6, 0) <= QT_VERSION && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    // 初始化子系统
    InstanceLibrary::initSubSystems();
    // 窗口图标
    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/qt/qml/friday_night_funkin/BF.ico"));

    GameLoop gameLoop;
    // 加载qml
    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("gameLoop", &gameLoop);
    engine.load(QUrl(QStringLiteral("qrc:/qt/qml/friday_night_funkin/QML/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;
    // 测试：放首歌听听
    auto test = new FunkinSound(false,":/mods/default/preload/music/freakyMenu/freakyMenu.ogg",ESoundType::uiSound,"freakyMenu");
    test->playSound();

    // 事件循环，防止执行到这窗口就没了
    return app.exec();
}