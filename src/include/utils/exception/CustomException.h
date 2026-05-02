//
// Created by kkplay on 4/23/26.
//

#pragma once
#include <string>
#include <QDebug>

using std::string;

enum class ModParseExcpetionType
{
    NoFileOrDir,
    BadJSONGrammer,
    OpenFileFail,
    GetWeekSongFailed,
    Unknown,
};

/**
 * 全局异常处理类，您不应该直接throw，那样会导致程序崩溃。我没有写任何try-catch。
 * 您应该调用下面的方法安全“抛出”异常，不影响后续代码执行。
*/
class Exception
{
public:
    static void logParseJSONException(const string& message,const string& functionName,const string& fileName,int atLine);
    static void logParseJSONException(const QString& message, const QString& functionName, const QString& fileName, int atLine);
    // 为了防止我写错逻错故意设置的自杀行为
    static void logNullPointerException(const string& message,const string& functionName,int atLine);
    static void logParseModException(ModParseExcpetionType excpetionType,const QString& file);
    static void logVersionInvalid(const string& versionProvided,const string& versionNeeded,const string& fileName);
    static void logVersionInvalid(const QString& versionProvided, const QString& versionNeeded, const QString& fileName);
    // 为了防止我写错逻错故意设置的自杀行为
    static void wrongParamException(const QString& msg,const QString& functionName, const int atLine);
};

// 自动获取函数名和行号，使用的时候直接LOG_JSON_PARSE_ERROR(msg,filename)这两个msg和filename替换一下即可
#define LOG_JSON_PARSE_ERROR(msg,filename) \
    Exception::logParseJSONException(msg,__FUNCTION__,filename,__LINE__);

#define LOG_JSON_PARSE_ERROR_Q(msg, fileName) \
    Exception::logParseJSONException(msg, __FUNCTION__, fileName, __LINE__)

#define LOG_NULL_PTR_ERROR(msg) \
    Exception::logNullPointerException(msg, __FUNCTION__, __LINE__)

#define LOG_WRONG_PARAM_ERROR(msg)  Exception::wrongParamException(msg, __FUNCTION__, __LINE__);