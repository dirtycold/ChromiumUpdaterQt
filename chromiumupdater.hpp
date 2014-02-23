
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

class ChromiumUpdater : public QObject
{
    Q_OBJECT
public:
    explicit ChromiumUpdater(QObject *parent = 0)
        :QObject(parent)
    {
        m_versionQueried = false;
    }
    ChromiumUpdater::~ChromiumUpdater()
    {
    }

    enum Action{ QueryVersion, DownloadInstaller};
    enum Platform{ Win32, Win64};
    enum Protocol{ Http, Https};


signals:
    void versionQueried();
    void installerDownloaded();
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
        QString urlString = getProtocolString() + "://" + m_baseUrl + "/" + getPlatformString() + "/" + QString::number(version()) + "/" + "mini_installer.exe";
        QUrl url(urlString);
        QNetworkRequest request(url);
        QSslConfiguration config = QSslConfiguration::defaultConfiguration();
        config.setProtocol(QSsl::AnyProtocol);
        request.setSslConfiguration(config);
        reply = m_accessManager.get(request);
        connect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SIGNAL(downloadProgress(qint64,qint64)));
        connect(reply,SIGNAL(finished()),this,SLOT(extractInstaller()));
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
        disconnect(&m_accessManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(extractVersion(QNetworkReply*)));
        emit versionQueried();
    }

    void extractInstaller()
    {
        QFile file("chromium_mini_installer_"+ QString::number(version()) + ".exe");
        if (!file.open(QIODevice::WriteOnly))
            return;
        file.write(reply->readAll());
        file.close();
        reply->deleteLater();
        QFileInfo fileinfo(file);
        m_filepath = fileinfo.absoluteFilePath();
        disconnect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SIGNAL(downloadProgress(qint64,qint64)));
        disconnect(reply,SIGNAL(finished()),this,SLOT(extractInstaller()));
        emit installerDownloaded();
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
    QNetworkReply *reply;
    QString m_baseUrl;
    Platform m_platform;
    Protocol m_protocol;

    unsigned int m_version;
    bool m_versionQueried;
    bool m_baseUrlSet;
    QString m_filepath;
};

#endif // CHROMIUMUPDATER_H
