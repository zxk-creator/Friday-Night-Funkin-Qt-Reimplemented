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

    static void logWarning(const QString& msg, const QString& fromWhere)
    {
        QString w = "[" + fromWhere + "] " + "警告: " + msg;
        addMessage(ErrType::Warning, w);
        qWarning() << w;
    }

    static void logWarning(const std::string& msg, const QString& fromWhere)
    {
        std::string w = "[" + fromWhere.toStdString() + "] " + "警告: " + msg;
        QString res = QString::fromStdString(w);
        addMessage(ErrType::Warning, res);
        qWarning() << QString::fromStdString(w);
    }

    static void logError(const QString& msg, const QString& fromWhere)
    {
        QString e = "[" + fromWhere + "] " + "错误: " + msg;
        addMessage(ErrType::Error, e);
        qCritical() << e;
    }

    // TODO:目前还没想好怎么把他显示上去，因此先保留空实现。
    static void printMessage()
    {

    }
};
