#include "profilepicturemanager.h"

#include <QImageReader>
#include <QNetworkReply>

ProfilePictureManager ProfilePictureManager::instance;

void ProfilePictureDownloadTask::doNetworkRequest() {
    QNetworkRequest request (url);
    QNetworkReply* reply = manager.networkManager().get(request);
    connect(reply, &QNetworkReply::finished, this, &ProfilePictureDownloadTask::onNetworkRequestFinished);
}

void ProfilePictureDownloadTask::onNetworkRequestFinished() {
    QImageReader imageReader ((QNetworkReply*) sender());
    imageReader.setScaledSize(QSize(32, 32));
    QImage res (32, 32, QImage::Format_RGB32);
    if (imageReader.read(&res)) {
        printf("got image!\n");
        emit imageAvailable(res);
    }
}