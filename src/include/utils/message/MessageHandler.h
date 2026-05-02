//
// Created by kkplay on 4/29/26.
//

#pragma once
#include <qdatetime.h>
#include <QDebug>

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
     * 任何一个方法都不用加上"提示"等前缀，我这里已经加好了。
     */
    static void logInfo(QString msg)
    {
        QString i = "提示: " + msg;
        addMessage(ErrType::Info,i);
        qInfo() << msg;
    }

    static void logWarning(QString msg)
    {
        QString w = "警告: " + msg;
        addMessage(ErrType::Warning,w);
        qWarning() << msg;
    }

    static void logWarning(std::string msg)
    {
        std::string e = "错误: " + msg;
        QString res = QString::fromStdString(e);
        addMessage(ErrType::Warning,res);
        qWarning() << res;
    }

    static void logError(QString msg)
    {
        QString e = "错误: " + msg;
        addMessage(ErrType::Error,e);
        qCritical() << msg;
    }

    // TODO:目前还没想好怎么把他显示上去，因此先保留空实现。
    static void printMessage()
    {

    }
};
