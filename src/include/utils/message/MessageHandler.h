//
// Created by kkplay on 4/29/26.
//

#pragma once
#include <qdatetime.h>
#include <QDebug>
#include <QMessageBox>

enum class ErrType
{
    Info,
    Warning,
    Error
};

struct LogEntry {
    ErrType type;
    QString message;
    QString timestamp;

    LogEntry(ErrType t, const QString& msg)
        : type(t), message(msg)
    {
        timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    }
};

// 全局消息处理机制，方便打印到屏幕上而非仅终端
class MessageHandler : QObject
{

    Q_OBJECT

    static constexpr int MAX_MESSAGE_LENGTH = 100;
    /**
     * 这也是单例模式，全局唯一
     * 每一个数组元素都是一个新的行
     */
    static inline QVector<LogEntry> msgBuffer;

    static void addMessage(ErrType type,QString& msg)
    {
        msgBuffer.append({type,msg});
        while (msgBuffer.size() > MAX_MESSAGE_LENGTH)
        {
            msgBuffer.removeFirst();
        }
    }

public:
    /**
     * 任何一个方法都不用加上"提示""[Where]"等前缀，我这里已经加好了。
     * @param msg 要加的消息
     * @param fromWhere 比如前缀"[Path]提示"中的Path
     */
    static void logInfo(const QString& msg,const QString& fromWhere)
    {
        QString i = "[" + fromWhere + "] " + "提示: " + msg;
        addMessage(ErrType::Info,i);
        qInfo() << i;
    }

    static void logWarning(bool showDialog,const QString& msg, const QString& fromWhere)
    {
        QString w = "[" + fromWhere + "] " + "警告: " + msg;
        addMessage(ErrType::Warning, w);
        qWarning() << w;

        // 调试模式下，一定弹出，这是给我看的。发布模式，特定情况才会给用户看
        #ifdef QT_DEBUG
        #else
        if (showDialog)
        #endif
            //QMessageBox::warning(nullptr,"警告",w,QMessageBox::Ok);
    }

    static void logError(bool showDialog,const QString& msg, const QString& fromWhere)
    {
        QString e = "[" + fromWhere + "] " + "错误: " + msg;
        addMessage(ErrType::Error, e);
        qCritical() << e;
        // 调试模式下，一定弹出，这是给我看的。发布模式，特定情况才会给用户看
        #ifdef QT_DEBUG
        #else
        if (showDialog)
        #endif
        //QMessageBox::critical(nullptr,"错误",e,QMessageBox::Ok);
    }
};

class LogProxy : public QObject
{
public:
    Q_OBJECT
public:
    explicit LogProxy(QObject* parent = nullptr) : QObject(parent) {}

    Q_INVOKABLE void logInfo(const QString& msg, const QString& fromWhere)
    {
        QString res = "[ QML: " + fromWhere + " ] ";
        MessageHandler::logInfo(msg, res);
    }

    Q_INVOKABLE void logWarning(const QString& msg, const QString& fromWhere)
    {
        QString res = "[ QML: " + fromWhere + " ] ";
        MessageHandler::logWarning(false,msg, res);
    }

    Q_INVOKABLE void logError(const QString& msg, const QString& fromWhere)
    {
        QString res = "[ QML: " + fromWhere + " ] ";
        MessageHandler::logError(false,msg, res);
    }

    // TODO: 获取所有的经过MessageHandler处理的信息，方便显示到屏幕上（尚未实现）
    Q_INVOKABLE void getMessages(){

    }
};

#define LOG_INFO(msg)    MessageHandler::logInfo(msg, Q_FUNC_INFO)
#define LOG_WARNING(showDialog,msg) MessageHandler::logWarning(showDialog,msg, Q_FUNC_INFO)
#define LOG_ERROR(showDialog,msg)   MessageHandler::logError(showDialog,msg, Q_FUNC_INFO)
#define LOG_PARSE_JSON_SUCCESS(whichType,filename) MessageHandler::logInfo(QString("成功解析了") + whichType + "，文件: " + filename, Q_FUNC_INFO)
#define LOG_NO_SUCH_FILE_ERROR(filepath) MessageHandler::logWarning(false,QString("没有这样的文件: ") + filepath, Q_FUNC_INFO);
