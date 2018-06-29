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

void ProfilePictureDownloadTask::start() {
    ProfilePictureManager::CacheInfo cacheInfo;
    if (QFile::exists(manager.getCachedImagePath(cid)) && QFile::exists(manager.getCachedMetaPath(cid))) {
        cacheInfo = manager.readCachedMeta(manager.getCachedMetaPath(cid));
        emit imageAvailable(QImage(manager.getCachedImagePath(cid)));
    }
    doNetworkRequest();
}

void ProfilePictureDownloadTask::doNetworkRequest() {
    QNetworkRequest request (url);
    QNetworkReply* reply = manager.networkManager().get(request);
    connect(reply, &QNetworkReply::finished, this, &ProfilePictureDownloadTask::onNetworkRequestFinished);
}

void ProfilePictureDownloadTask::onNetworkRequestFinished() {
    QImageReader imageReader ((QNetworkReply*) sender());
    imageReader.setScaledSize(QSize(32, 32));
    QImage res (32, 32, QImage::Format_RGB32);
    if (imageReader.read(&res))
        emit imageAvailable(res);
}