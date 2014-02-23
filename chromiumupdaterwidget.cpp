
/*
  ChromiumUpdaterQt - A Chromium Updater written in Qt.
  Copyright (c) 2014 ZH (zhanghan@gmx.cn)
*/

#include "chromiumupdaterwidget.h"

#include <QLayout>
#include <QLabel>

ChromiumUpdaterWidget::ChromiumUpdaterWidget(QWidget *parent) :
    QWidget(parent)
{    
    QHBoxLayout *hLayout = new QHBoxLayout();
    QVBoxLayout *vLayout = new QVBoxLayout(this);

    m_checkButton = new QPushButton("Check Version",this);
    m_downloadButton = new QPushButton ("Download and Install",this);
    m_downloadButton->setEnabled(false);

    hLayout->addWidget(m_checkButton);
    hLayout->addWidget(m_downloadButton);

    m_statusBar = new QProgressBar(this);
    m_statusBar->setAlignment(Qt::AlignCenter);
    m_statusBar->setFormat("Ready");
    m_statusBar->setValue(0);

    QLabel *label = new QLabel(this);
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
    connect(&m_updater,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(process(qint64,qint64)));
    connect(&m_updater,SIGNAL(installerDownloaded()),this,SLOT(install()));
}

ChromiumUpdaterWidget::~ChromiumUpdaterWidget()
{
    m_setting->sync();
    delete m_checkButton;
    delete m_downloadButton;
    delete m_statusBar;
    delete m_setting;
}

void ChromiumUpdaterWidget::versionQueried()
{
    unsigned int version = m_updater.version();
    unsigned int last_version = m_setting->value("Version").toUInt();
    if (version != 0)
    {
        if (version > last_version)
        {
            m_setting->setValue("Version",version);
            m_statusBar->setFormat("Version: " + QString::number(version));
            m_downloadButton->setEnabled(true);
        }
        else
        {
            m_statusBar->setFormat("No newer version available.");
        }
    }
    else
    {
        m_statusBar->setFormat("Version quering failed.");
    }
}

void ChromiumUpdaterWidget::checkClicked()
{
    m_downloadButton->setEnabled(false);
    m_updater.queryVersion();
}

void ChromiumUpdaterWidget::downloadClicked()
{
    m_checkButton->setEnabled(false);
    m_updater.downloadInstaller();
}

void ChromiumUpdaterWidget::process(qint64 current, qint64 total)
{
    int percent = current * 100 / total;
    m_statusBar->setFormat("Download in progress: " + QString::number(percent) + "%");
    m_statusBar->setValue(percent);
}

void ChromiumUpdaterWidget::install()
{
    QString filepath;
    filepath = m_updater.installerPath();
    m_checkButton->setEnabled(true);
    m_statusBar->setFormat("Silent Installation in background. You can now close the updater. " + filepath);
    //actual installation
}
