
/*
  ChromiumUpdaterQt - A Chromium Updater written in Qt.
  Copyright (c)2014 - 2015 ZH (zhanghan@gmx.cn)
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
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QGridLayout *mainLayout = new QGridLayout(this);

    m_checkButton = new QPushButton("Check Version",this);
    m_checkButton->setEnabled(true);
    m_checkButton->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload)));
    m_checkButton->setToolTip("Check for new version");

    m_downloadButton = new QPushButton ("Download and Install",this);
    m_downloadButton->setEnabled(false);
    m_downloadButton->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogApplyButton)));
    m_downloadButton->setToolTip("Download and install");

    buttonLayout->addWidget(m_checkButton);
    buttonLayout->addWidget(m_downloadButton);

    m_urlButton = new QPushButton("",this);
    m_urlButton->setEnabled(false);
    m_urlButton->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton)));
    m_urlButton->setToolTip("Copy source url");
    m_urlButton->setFixedWidth(24);

    m_statusBar = new QProgressBar(this);
    m_statusBar->setAlignment(Qt::AlignCenter);
    m_statusBar->setFormat("Ready");
    m_statusBar->setValue(0);
    m_statusBar->setMinimumWidth(240);

    QLabel *label = new QLabel(this);
    label->setMaximumHeight(20);
    label->setText("A Chromium Updater written in Qt. (c)2014 - 2015 ZH");

    mainLayout->addWidget(m_statusBar,0,0);
    mainLayout->addWidget(m_urlButton,0,1);
    mainLayout->addLayout(buttonLayout,1,0);
    mainLayout->addWidget(label,2,0);

    if (!m_updater.supportsSsl())
        m_statusBar->setFormat("SSL-support missing. HTTPS connection not possible.");

    m_setting = new QSettings("settings.ini", QSettings::IniFormat, this);
    if (!m_setting->contains("Version"))
        m_setting->setValue("Version",0);
    if (!m_setting->contains("BaseUrl"))
        m_setting->setValue("BaseUrl","commondatastorage.googleapis.com/chromium-browser-continuous");
    if (!m_setting->contains("Platform"))
        m_setting->setValue("Platform","Win32");
    if (!m_setting->contains("Protocol"))
        m_setting->setValue("Protocol","HTTPS");
    if (!m_setting->contains("AutoCheck"))
        m_setting->setValue("AutoCheck",true);
    if (!m_setting->contains("AutoDownload"))
        m_setting->setValue("AutoDownload",false);
    if (!m_setting->contains("AutoRemove"))
        m_setting->setValue("AutoRemove",false);
    if (!m_setting->contains("UseSystemProxy"))
        m_setting->setValue("UseSystemProxy",true);
    if (!m_setting->contains("UseManualProxy"))
        m_setting->setValue("UseManualProxy",false);
    if (!m_setting->contains("ManualProxyType"))
        m_setting->setValue("ManualProxyType","HTTP");
    if (!m_setting->contains("ManualProxyHost"))
        m_setting->setValue("ManualProxyHost","");
    if (!m_setting->contains("ManualProxyPort"))
        m_setting->setValue("ManualProxyPort",0);
    if (!m_setting->contains("ManualProxyUsername"))
        m_setting->setValue("ManualProxyUsername","");
    if (!m_setting->contains("ManualProxyPassword"))
        m_setting->setValue("ManualProxyPassword","");


    m_baseUrl = m_setting->value("BaseUrl").toString();
    m_updater.setBaseUrl(m_baseUrl);
    m_autoCheck = m_setting->value("AutoCheck").toBool();
    m_autoDownload = m_setting->value("AutoDownload").toBool();
    m_autoRemove = m_setting->value("AutoRemove").toBool();
    m_useSystemProxy = m_setting->value("UseSystemProxy").toBool();
    m_useManualProxy = m_setting->value("UseManualProxy").toBool();
    m_manualProxyType = m_setting->value("ManualProxyType").toString();
    m_manualProxyHost = m_setting->value("ManualProxyHost").toString();
    m_manualProxyPort = m_setting->value("ManualProxyPort").toInt();
    m_manualProxyUsername = m_setting->value("ManualProxyUsername").toString();
    m_manualProxyPassword = m_setting->value("ManualProxyPassword").toString();

    // better use QMetaEnum?
    ChromiumUpdater::Platform platform = ChromiumUpdater::Win32;
    if(m_setting->value("Platform").toString() == "Win64")
        platform = ChromiumUpdater::Win64;
    m_updater.setPlatform(platform);

    ChromiumUpdater::Protocol protocol = ChromiumUpdater::HTTPS;
    if (m_setting->value("Protocol").toString().toUpper() == "HTTP")
        protocol = ChromiumUpdater::HTTP;
    m_updater.setProtocol(protocol);

    // after setting those parameters to Updater class
    // call setSystemProxySetting()
    if (m_useSystemProxy && !m_useManualProxy)
    {
        m_updater.setSystemProxySetting();
    }
    else if (!m_useSystemProxy && m_useManualProxy)
    {
        m_updater.setManualProxySetting(m_manualProxyType, m_manualProxyHost, m_manualProxyPort, m_manualProxyUsername, m_manualProxyPassword);
    }
    // otherwise will use direct connection.

    connect(m_checkButton,SIGNAL(clicked()),this,SLOT(checkClicked()));
    connect(&m_updater,SIGNAL(versionQueried()),this,SLOT(versionQueried()));
    connect(m_downloadButton,SIGNAL(clicked()),this,SLOT(downloadClicked()));
    connect(&m_updater,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(downloadProgress(qint64,qint64)));
    connect(&m_updater,SIGNAL(installerDownloaded()),this,SLOT(downloadComplete()));
    connect(&m_updater,SIGNAL(installComplete(int)),this,SLOT(installComplete(int)));
    connect(this,SIGNAL(readyToInstall()),this,SLOT(install()));
    connect(m_urlButton,SIGNAL(clicked()),this,SLOT(copyUrl()));

    if (m_autoCheck)
    {
        m_checkButton->setVisible(false);
        emit checkClicked();
    }
    if (m_autoDownload)
    {
        m_downloadButton->setVisible(false);
    }
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
        if (m_autoDownload)
            emit downloadClicked();
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
    //common behaviour
    m_checkButton->setEnabled(true);

    if (code == 0) // installer finished ok
    {
        m_downloadButton->setEnabled(true);
        m_statusBar->setFormat("Installation complete.");
        m_statusBar->setRange(0,100);
        m_statusBar->setValue(100);
        m_setting->setValue("Version",m_updater.version());
        if (m_autoRemove)
            m_updater.removeInstaller();
    }
    else // not ok
    {
        m_downloadButton->setEnabled(false);
        m_statusBar->setFormat("Installation failed.");
        m_statusBar->setRange(0,100);
        m_statusBar->setValue(0);
        m_updater.removeInstaller();
    }
}

void ChromiumUpdaterWidget::copyUrl()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(m_updater.installerUrl());
}
