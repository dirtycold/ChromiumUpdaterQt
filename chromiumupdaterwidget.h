
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
    void readyToInstall();

public slots:
    void versionQueried();
    void checkClicked();
    void downloadClicked();
    void downloadProgress(qint64 , qint64 );
    void downloadComplete();
    void install();
    void installComplete(int code);
    void copyUrl();

private:
    ChromiumUpdater m_updater;

    QPushButton *m_checkButton;
    QPushButton *m_downloadButton;
    QPushButton *m_urlButton;
    QProgressBar *m_statusBar;

    QSettings *m_setting;
    QString m_baseUrl;
    bool m_autoCheck;
    bool m_autoDownload;
    bool m_autoRemove;
    bool m_useSystemProxy;

};

#endif // CHROMIUMUPDATERWIDGET_H
