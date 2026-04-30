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

    static void logInfo(QString msg)
    {
        addMessage(ErrType::Info,msg);
        qInfo() << msg;
    }

    static void logWarning(QString msg)
    {
        addMessage(ErrType::Warning,msg);
        qWarning() << msg;
    }

    static void logError(QString msg)
    {
        addMessage(ErrType::Error,msg);
        qCritical() << msg;
    }

    // TODO:目前还没想好怎么把他显示上去，因此先保留空实现。
    static void printMessage()
    {

    }
};
