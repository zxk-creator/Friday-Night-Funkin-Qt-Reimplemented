//
// Created by kkplay on 4/23/26.
//

#include "utils/exception/CustomException.h"
#include "utils/message/MessageHandler.h"

void Exception::logParseJSONException(const string& message, const string& functionName, const string& fileName, int atLine)
{
    string resStr = "\nJSON解析错误！\n" + message + "\n发生在函数 " + functionName + " 行 " + std::to_string(atLine) + "\nJSON文件名：" + fileName + "\n";
    MessageHandler::logError(QString::fromStdString(resStr), QString::fromStdString(fileName));

    // TODO: 也许我们可以在这里加一些弹窗逻辑告知用户？
}

void Exception::logParseJSONException(const QString& message, const QString& functionName, const QString& fileName, int atLine)
{
    QString resStr = "\nJSON解析错误！\n" + message + "\n发生在函数 " + functionName + " 行 " + QString::number(atLine) + "\nJSON文件名：" + fileName + "\n";
    MessageHandler::logError(resStr, "Exception");

    // TODO: 也许我们可以在这里加一些弹窗逻辑告知用户？
}

void Exception::logNullPointerException(const string& message, const string& functionName, int atLine)
{
    string resStr = "\n发生空指针异常！\n" + message + "\n发生在函数" + functionName + "行" + std::to_string(atLine);
    MessageHandler::logError(QString::fromStdString(resStr), "Exception");

    // 都空指针了再运行岂不是炸了
    std::terminate();
}

void Exception::logParseModException(ModParseExcpetionType excpetionType, const QString& file)
{
    QString message;
    switch (excpetionType)
    {
    case ModParseExcpetionType::NoFileOrDir:
        message = "模组解析失败: 没有这样的文件或文件夹" + file;
        break;
    case ModParseExcpetionType::BadJSONGrammer:
        message = "模组解析失败: JSON语法错误" + file;
        break;
    case ModParseExcpetionType::OpenFileFail:
        message = "文件打开失败：" + file;
        break;
    case ModParseExcpetionType::GetWeekSongFailed:
        message = "尝试从周配置信息读取歌曲时失败。文件：" + file;
        break;
    default:
        message = "模组解析失败: 未知错误" + file;
        break;
    }

    MessageHandler::logError(message, "Exception");
}

void Exception::logVersionInvalid(const string& versionProvided, const string& versionNeeded, const string& fileName)
{
    string message = "模组版本不匹配，需要" + versionProvided + "，却提供了: " + versionNeeded + "。在文件: " + fileName;
    MessageHandler::logError(QString::fromStdString(message), "Exception");
}

void Exception::logVersionInvalid(const QString& versionProvided, const QString& versionNeeded, const QString& fileName)
{
    QString message = "模组版本不匹配，需要" + versionProvided + "，却提供了: " + versionNeeded + "。在文件: " + fileName;
    MessageHandler::logError(message, "Exception");
}

void Exception::wrongParamException(const QString& msg, const QString& functionName, const int atLine)
{
    QString res = msg + "，发生在" + functionName + "，在" + QString::number(atLine) + "行";
    MessageHandler::logError(res, "Exception");

    // 直接自杀
    std::terminate();
}

void Exception::killGame(const QString& why,const QString& fromWhere)
{
    MessageHandler::logError(why,fromWhere);
    std::terminate();
}
