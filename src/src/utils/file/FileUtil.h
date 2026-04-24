//
// Created by kkplay on 4/24/26.
//

#pragma once
#include <QDir>
#include <QFile>
#include <qiodevice.h>
#include <QString>

#include "utils/exception/CustomException.h"

namespace FileUtil
{

    /**
     * 一个函数打开文件
     * @param fileAbsolutePath 文件绝对路径，一定要是文件路径！
     * @return 转换后的字符串
     */
    QString ReadFileToString(const QString& fileAbsolutePath)
    {
        if (fileAbsolutePath.isEmpty()) {
            qWarning("传入了空路径！在ReadFileToString函数");
            return "";
        }
        QFile file(fileAbsolutePath);
        if (!file.exists())
        {
            Exception::logParseModException(ModParseExcpetionType::NoFileOrDir, fileAbsolutePath);
            return "";
        }
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            Exception::logParseModException(ModParseExcpetionType::OpenFileFail,file.fileName());
            return "";
        }

        QByteArray data = file.readAll();
        QString contents = QString::fromUtf8(data);
        file.close();
        return contents;
    }
}
