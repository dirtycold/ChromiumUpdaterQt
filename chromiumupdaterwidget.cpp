
/*
  ChromiumUpdaterQt - A Chromium Updater written in Qt.
  Copyright (c) 2014 ZH (zhanghan@gmx.cn)
*/

#include "chromiumupdaterwidget.h"

#include <QLayout>
#include <QLabel>
#include <QClipboard>
#include <QApplication>
#include <QStyle>
#include <QSizePolicy>

ChromiumUpdaterWidget::ChromiumUpdaterWidget(QWidget *parent) :
    QWidget(parent)
{    
    QHBoxLayout *hLayout = new QHBoxLayout();
    QVBoxLayout *vLayout = new QVBoxLayout(this);

    m_checkButton = new QPushButton("Check Version",this);
    m_downloadButton = new QPushButton ("Download and Install",this);
    m_urlButton = new QPushButton("",this);
    m_downloadButton->setEnabled(false);
    m_urlButton->setEnabled(false);
    m_checkButton->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload)));
    m_downloadButton->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogApplyButton)));
    m_urlButton->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton)));
    m_checkButton->setToolTip("Check for new version");
    m_downloadButton->setToolTip("Download and install");
    m_urlButton->setToolTip("Copy source url");
    m_urlButton->setFixedWidth(24);

    hLayout->addWidget(m_checkButton);
    hLayout->addWidget(m_downloadButton);
    hLayout->addWidget(m_urlButton);

    m_statusBar = new QProgressBar(this);
    m_statusBar->setAlignment(Qt::AlignCenter);
    m_statusBar->setFormat("Ready");
    m_statusBar->setValue(0);

    QLabel *label = new QLabel(this);
    label->setMaximumHeight(20);
    label->setText("A Chromium Updater written in Qt. (c)2014 ZH");

    vLayout->insertLayout(0,hLayout);
    vLayout->insertWidget(1,m_statusBar);
    vLayout->insertWidget(2,label);

    if (!m_updater.supportsSsl())
        m_statusBar->setFormat("SSL-support missing. HTTPS connection not possible.");

    m_setting = new QSettings("settings.ini", QSettings::IniFormat, this);
    if (!m_setting->contains("Version"))
        m_setting->setValue("Version",0);
    if (!m_setting->contains("BaseUrl"))
        m_setting->setValue("BaseUrl","commondatastorage.googleapis.com/chromium-browser-continuous");

    m_baseUrl = m_setting->value("BaseUrl").toString();
    m_updater.setBaseUrl(m_baseUrl);

    connect(m_checkButton,SIGNAL(clicked()),this,SLOT(checkClicked()));
    connect(&m_updater,SIGNAL(versionQueried()),this,SLOT(versionQueried()));
    connect(m_downloadButton,SIGNAL(clicked()),this,SLOT(downloadClicked()));
    connect(&m_updater,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(downloadProgress(qint64,qint64)));
    connect(&m_updater,SIGNAL(installerDownloaded()),this,SLOT(downloadComplete()));
    connect(&m_updater,SIGNAL(installComplete(int)),this,SLOT(installComplete(int)));
    connect(this,SIGNAL(readyToInstall()),this,SLOT(install()));
    connect(m_urlButton,SIGNAL(clicked()),this,SLOT(copyUrl()));
}

ChromiumUpdaterWidget::~ChromiumUpdaterWidget()
{
    m_setting->sync();
    delete m_checkButton;
    delete m_downloadButton;
    delete m_urlButton;
    delete m_statusBar;
    delete m_setting;
}

void ChromiumUpdaterWidget::versionQueried()
{
    unsigned int version = m_updater.version();
    unsigned int last_version = m_setting->value("Version").toUInt();
    if (version != 0)
    {
        if (m_updater.installerExists())
        {
            if (version > last_version)
            {
                // direct install
                m_statusBar->setFormat("Downloaded new version: " + QString::number(version) + ". Ready to install.");
                m_downloadButton->setEnabled(true);
            }
            else if (version == last_version)
            {
                // re-install
                m_statusBar->setFormat("Installed version: " + QString::number(version) + ". Ready to re-install.");
                m_downloadButton->setEnabled(true);
            }
            else // version < last_version
            {
                // install older version
                m_statusBar->setFormat("Old version: " + QString::number(version) + ". Check url or proceed to install older version.");
                m_downloadButton->setEnabled(true);
            }
        }
        else
        {
            if (version > last_version)
            {
                // fresh install
                m_statusBar->setFormat("New version: " + QString::number(version) + ". Ready to download.");
                m_downloadButton->setEnabled(true);
            }
            else if (version == last_version)
            {
                // re-download
                m_statusBar->setFormat("Installed version: " + QString::number(version) + ". Ready to re-download.");
                m_downloadButton->setEnabled(true);
            }
            else // version < last_version
            {
                // download older version
                m_statusBar->setFormat("Old version: " + QString::number(version) + ". Check url or proceed to download older version.");
                m_downloadButton->setEnabled(true);
            }
        }

        m_urlButton->setEnabled(true);
    }
    else
    {
        m_statusBar->setFormat("Version quering failed.");
    }
}

void ChromiumUpdaterWidget::checkClicked()
{
    m_downloadButton->setEnabled(false);
    m_urlButton->setEnabled(false);
    m_updater.queryVersion();
}

void ChromiumUpdaterWidget::downloadClicked()
{
    if ( m_updater.installerExists())
    {
        emit readyToInstall();
        return;
    }

    m_checkButton->setEnabled(false);
    m_downloadButton->setEnabled(false);
    m_updater.downloadInstaller();
}

void ChromiumUpdaterWidget::downloadProgress(qint64 current, qint64 total)
{
    int percent = current * 100 / total;
    m_statusBar->setFormat("Download in progress: " + QString::number(percent) + "%");
    m_statusBar->setValue(percent);
}

void ChromiumUpdaterWidget::downloadComplete()
{
    m_checkButton->setEnabled(true);
    m_downloadButton->setEnabled(false);
    m_statusBar->setFormat("Download complete.");
    emit readyToInstall();
}

void ChromiumUpdaterWidget::install()
{
    m_checkButton->setEnabled(false);
    m_downloadButton->setEnabled(false);

    m_statusBar->setTextVisible(true);
    m_statusBar->setFormat("Installation in progress.");
    m_statusBar->setRange(0,0); // show busy indicator.
    m_updater.install();
}

void ChromiumUpdaterWidget::installComplete(int code)
{
    if (code == 0)
    {
        m_checkButton->setEnabled(true);
        m_downloadButton->setEnabled(true);
        //assume installation ok.
        m_statusBar->setFormat("Installation complete.");
        m_statusBar->setRange(0,100);
        m_statusBar->setValue(100);
        m_setting->setValue("Version",m_updater.version());
    }
}

void ChromiumUpdaterWidget::copyUrl()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(m_updater.installerUrl());
}
