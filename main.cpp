
/*
  ChromiumUpdaterQt - A Chromium Updater written in Qt.
  Copyright (c)2014 - 2015 ZH (zhanghan@gmx.cn)
*/

#include "chromiumupdaterwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ChromiumUpdaterWidget w;
    w.show();

    return a.exec();
}
