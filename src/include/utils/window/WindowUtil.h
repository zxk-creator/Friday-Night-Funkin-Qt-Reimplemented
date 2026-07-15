//
// Created by kkplay on 7/15/26.
//

#pragma once
#include <QMessageBox>

namespace WindowUtil
{
    inline void showQuickDialog(QMessageBox::Icon type, const QString& title, const QString& msg)
    {
        QMessageBox err;
        err.setIcon(type);
        err.setWindowTitle(title);
        err.setText(msg);
        err.setDefaultButton(QMessageBox::Ok);
        err.setStandardButtons(QMessageBox::Ok);
        err.exec();
    }
}
