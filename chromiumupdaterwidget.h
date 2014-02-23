
/*
  ChromiumUpdaterQt - A Chromium Updater written in Qt.
  Copyright (c) 2014 ZH (zhanghan@gmx.cn)
*/

#ifndef CHROMIUMUPDATERWIDGET_H
#define CHROMIUMUPDATERWIDGET_H

#include <QWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>

#include "chromiumupdater.hpp"

class ChromiumUpdaterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ChromiumUpdaterWidget(QWidget *parent = 0);
    ~ChromiumUpdaterWidget();

signals:

public slots:
    void versionQueried();
    void checkClicked();
    void downloadClicked();
    void process(qint64 , qint64 );
    void install();

private:
    ChromiumUpdater m_updater;

    QPushButton *m_checkButton;
    QPushButton *m_downloadButton;
    QProgressBar *m_statusBar;

    QSettings *m_setting;
    QString m_baseUrl;

};

#endif // CHROMIUMUPDATERWIDGET_H
