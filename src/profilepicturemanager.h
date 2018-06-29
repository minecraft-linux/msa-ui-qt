#ifndef PROFILEPICTUREMANAGER_H
#define PROFILEPICTUREMANAGER_H

#include <string>
#include <QObject>
#include <QRunnable>
#include <QNetworkAccessManager>

class ProfilePictureManager {

public:
    struct CacheInfo {
        QString url;
        QString etag;
    };

private:
    QNetworkAccessManager networkAccessManagerInstance;
    QString cacheBaseDir;

public:
    static ProfilePictureManager instance;

    ProfilePictureManager();

    QNetworkAccessManager& networkManager() { return networkAccessManagerInstance; }

    QString getCachedMetaPath(QString const& cid);
    QString getCachedImagePath(QString const& cid);

    CacheInfo readCachedMeta(QString const& filePath);
    void writeCachedMeta(QString const& filePath, CacheInfo const& cacheInfo);

    void cacheImage(QString const& cid, CacheInfo const& cacheInfo, QImage const& image);
    void removeCachedImage(QString const& cid);

};

class ProfilePictureDownloadTask : public QObject {
    Q_OBJECT

    ProfilePictureManager& manager;
    QString cid, url;

    void doNetworkRequest(ProfilePictureManager::CacheInfo const& cacheInfo);

    void onNetworkRequestFinished();

    static QImage cropCircleImage(QImage const& source);

public:
    ProfilePictureDownloadTask(QString cid, QString url, QObject* parent = nullptr,
                               ProfilePictureManager& manager = ProfilePictureManager::instance)
            : QObject(parent), manager(manager), cid(std::move(cid)), url(std::move(url)) {}

    void start();

signals:
    void imageAvailable(QImage const& image);

};

#endif //PROFILEPICTUREMANAGER_H
