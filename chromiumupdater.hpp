
/*
  ChromiumUpdaterQt - A Chromium Updater written in Qt.
  Copyright (c) 2014 ZH (zhanghan@gmx.cn)
*/

#ifndef CHROMIUMUPDATER_H
#define CHROMIUMUPDATER_H

#include <QObject>
#include <QString>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QSslSocket>
#include <QtNetwork/QSslConfiguration>
#include <QFile>
#include <QFileInfo>
#include <QProcess>

class ChromiumUpdater : public QObject
{
    Q_OBJECT
public:
    explicit ChromiumUpdater(QObject *parent = 0)
        :QObject(parent)
    {
        m_versionQueried = false;
        connect(&m_installer,SIGNAL(finished(int)),this,SIGNAL(installComplete(int)));
    }
    ChromiumUpdater::~ChromiumUpdater()
    {
       // m_installer.waitForFinished();
        m_installer.kill();
    }

    enum Platform{ Win32, Win64};
    enum Protocol{ Http, Https};


signals:
    void versionQueried();
    void installerDownloaded();
    void installComplete(int);
    void installAborted();
    void downloadProgress(qint64,qint64);

public slots:
    static inline bool supportsSsl()
    {
        return QSslSocket::supportsSsl();
    }

    void queryVersion()
    {
        if (!m_baseUrlSet)
            return;
        connect(&m_accessManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(extractVersion(QNetworkReply*)));
        QString urlString = getProtocolString() + "://" + m_baseUrl + "/" + getPlatformString() + "/" + "LAST_CHANGE";
        QUrl url(urlString);
        QNetworkRequest request(url);
        QSslConfiguration config = QSslConfiguration::defaultConfiguration();
        config.setProtocol(QSsl::AnyProtocol);
        request.setSslConfiguration(config);
        m_accessManager.get(request);
    }

    void downloadInstaller()
    {
        if (!hasVersionQueried())
            return;
        QUrl url(m_url);
        QNetworkRequest request(url);
        QSslConfiguration config = QSslConfiguration::defaultConfiguration();
        config.setProtocol(QSsl::AnyProtocol);
        request.setSslConfiguration(config);
        m_reply = m_accessManager.get(request);
        connect(m_reply,SIGNAL(downloadProgress(qint64,qint64)),this,SIGNAL(downloadProgress(qint64,qint64)));
        connect(m_reply,SIGNAL(finished()),this,SLOT(extractInstaller()));
    }

    void install()
    {
        if ((m_baseUrlSet && m_versionQueried) && m_installerDownloaded)
        {
            m_installer.start(m_filepath);
        }
    }

    bool hasVersionQueried()
    {
        return m_versionQueried;
    }

    unsigned int version()
    {
        if (!hasVersionQueried())
            return 0;
        else
            return m_version;
    }

    QString installerPath()
    {
        return m_filepath;
    }

    QString installerUrl()
    {
        return m_url;
    }

    bool installerExists()
    {
        QFileInfo fileinfo(m_filepath);
        return fileinfo.exists();
    }

    void setBaseUrl(QString baseUrl)
    {
        m_baseUrl = baseUrl;
    }
    void setPlatform(Platform platform)
    {
        m_platform = platform;
    }
    void setProtocol(Protocol protocol)
    {
        m_protocol = protocol;
    }

private slots:
    void extractVersion(QNetworkReply *reply)
    {
        QByteArray rawData = reply->readAll();
        unsigned int version = rawData.toUInt();
        reply->deleteLater();
        m_version = version;
        m_versionQueried = true;

        //determine installer file name and path
        QFile file("chromium_mini_installer_"+ QString::number(version) + ".exe");
        QFileInfo fileinfo(file);
        m_filepath = fileinfo.absoluteFilePath();
        m_url = getProtocolString() + "://" + m_baseUrl + "/" + getPlatformString() + "/" + QString::number(version) + "/" + "mini_installer.exe";
        disconnect(&m_accessManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(extractVersion(QNetworkReply*)));
        emit versionQueried();
    }

    void extractInstaller()
    {
        QFile file(m_filepath);
        if (!file.open(QIODevice::WriteOnly))
            return;
        file.write(m_reply->readAll());
        file.close();
        m_reply->deleteLater();
        disconnect(m_reply,SIGNAL(downloadProgress(qint64,qint64)),this,SIGNAL(downloadProgress(qint64,qint64)));
        disconnect(m_reply,SIGNAL(finished()),this,SLOT(extractInstaller()));
        emit installerDownloaded();
        m_installerDownloaded = true;
    }

    QString getPlatformString()
    {
        QString platformString;
        switch (m_platform) {
        case Win32:
            platformString = "Win";
            break;
        default:
        case Win64:
            platformString = "Win_x64";
            break;
        }
        return platformString;
    }

    QString getProtocolString()
    {
        QString protocolString;
        switch (m_protocol) {
        case Http:
            protocolString = "http";
            break;
        default:
        case Https:
            protocolString = "https";
            break;
        }
        return protocolString;
    }

private:
    QNetworkAccessManager m_accessManager;
    QNetworkReply *m_reply;
    QProcess m_installer;
    QString m_baseUrl;
    Platform m_platform;
    Protocol m_protocol;

    unsigned int m_version;
    bool m_versionQueried;
    bool m_baseUrlSet;
    bool m_installerDownloaded;
    QString m_filepath;
    QString m_url;
};

#endif // CHROMIUMUPDATER_H
