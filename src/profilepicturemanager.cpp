#include "profilepicturemanager.h"

#include <QImageReader>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>

ProfilePictureManager ProfilePictureManager::instance;

ProfilePictureManager::ProfilePictureManager() {
    cacheBaseDir = QDir(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation))
            .filePath("msa/profile_pictures");
    QDir().mkpath(cacheBaseDir);
}

QString ProfilePictureManager::getCachedImagePath(QString const& cid) {
    return QDir(cacheBaseDir).filePath(cid + ".png");
}

QString ProfilePictureManager::getCachedMetaPath(QString const& cid) {
    return QDir(cacheBaseDir).filePath(cid + ".png.meta");
}

ProfilePictureManager::CacheInfo ProfilePictureManager::readCachedMeta(QString const& filePath) {
    CacheInfo ret;
    QSettings settings (filePath, QSettings::IniFormat);
    ret.url = settings.value("CacheInfo/URL").toString();
    ret.etag = settings.value("CacheInfo/ETag").toString();
    return ret;
}

void ProfilePictureManager::writeCachedMeta(QString const& filePath, CacheInfo const& cacheInfo) {
    QSettings settings (filePath, QSettings::IniFormat);
    settings.setValue("CacheInfo/URL", cacheInfo.url);
    settings.setValue("CacheInfo/ETag", cacheInfo.etag);
}

void ProfilePictureManager::cacheImage(QString const& cid, CacheInfo const& cacheInfo, QImage const& image) {
    writeCachedMeta(getCachedMetaPath(cid), cacheInfo);
    image.save(getCachedImagePath(cid));
}

void ProfilePictureManager::removeCachedImage(QString const& cid) {
    QFile::remove(getCachedImagePath(cid));
    QFile::remove(getCachedMetaPath(cid));
}


void ProfilePictureDownloadTask::start() {
    ProfilePictureManager::CacheInfo cacheInfo;
    if (QFile::exists(manager.getCachedImagePath(cid)) && QFile::exists(manager.getCachedMetaPath(cid))) {
        cacheInfo = manager.readCachedMeta(manager.getCachedMetaPath(cid));
        emit imageAvailable(QImage(manager.getCachedImagePath(cid)));
    }
    doNetworkRequest(cacheInfo);
}

void ProfilePictureDownloadTask::doNetworkRequest(ProfilePictureManager::CacheInfo const& cacheInfo) {
    QNetworkRequest request (url);
    if (cacheInfo.url == url) {
        request.setRawHeader("If-None-Match", (QStringLiteral("\"") + cacheInfo.etag + QStringLiteral("\"")).toUtf8());
    }
    QNetworkReply* reply = manager.networkManager().get(request);
    connect(reply, &QNetworkReply::finished, this, &ProfilePictureDownloadTask::onNetworkRequestFinished);
}

void ProfilePictureDownloadTask::onNetworkRequestFinished() {
    QNetworkReply* reply = (QNetworkReply*) sender();
    int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (status == 200) {
        QImageReader imageReader(reply);
        imageReader.setScaledSize(QSize(32, 32));
        QImage res(32, 32, QImage::Format_RGB32);
        if (imageReader.read(&res)) {
            emit imageAvailable(res);

            ProfilePictureManager::CacheInfo cacheInfo;
            cacheInfo.url = url;
            cacheInfo.etag = reply->rawHeader("ETag");
            manager.cacheImage(cid, cacheInfo, res);
        }
    } else if (status == 404) {
        manager.removeCachedImage(cid);
    }
}