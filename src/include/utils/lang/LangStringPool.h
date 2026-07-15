//
// Created by kkplay on 4/16/26.
//

#pragma once
#include <QObject>

// 为了能够翻译成别的语言而设计的
class LangStringPool : public QObject
{
    Q_OBJECT

private:
    static inline LangStringPool* langStringPool;

public:
    static LangStringPool* instance()
    {
        if (!langStringPool) langStringPool = new LangStringPool();
        return langStringPool;
    }
    // 调试相关
    QString PEMod() const { return tr("PE模组"); }
    QString errWhenParse() const { return tr("无法解析PE模组"); }

    // 模组浏览界面的字
    QString untitledMod() const { return tr("未命名模组"); }
    QString noDescription() const { return tr("模组作者好像忘了命名...."); }
    QString modNeedEngineVersion() const { return tr("模组要求的引擎版本： <br/>"); }
    QString modVersion() const { return tr("模组版本: <br/>"); }
    QString modDependencies() const { return tr("模组依赖: <br/>"); }
    QString modOptionalDependencies() const { return tr("模组可选依赖: <br/>"); }
    QString modHomePage() const { return tr("模组下载网站: <br/>"); }
    QString modLicense() const { return tr("模组许可证: <br/>"); }
    QString version() const { return tr("版本: "); }
    QString description() const { return tr("描述: <br/>"); }
    QString homepage() const { return tr("下载网站: <br/>"); }
    QString contributor() const { return tr("贡献者: <br/>"); }

    // 解析模组相关
    QString unknownWeekName() const { return tr("未命名周目"); }
    QString unknownAuthor() const { return  tr("一个神奇的PE作者"); }

    // 错误和异常相关
    QString scriptRuntimeError() const { return tr("脚本运行时错误！"); }
    QString scriptRuntimeErrorMsg() const { return tr("脚本执行过程中发生了无法恢复的异常: %1 。请检查脚本语法和逻辑是否正确，然后再试一次。"); }
    QString fatalError() const { return tr("错误！"); }
    QString fatalStartupErrorMsg() const { return tr("应用程序在启动过程中发生了无法处理的致命异常，正在退出...."); }
    QString fatalRuntimeErrorMsg() const { return tr("应用程序在运行过程中发生了无法处理的致命异常，正在退出...."); }
};