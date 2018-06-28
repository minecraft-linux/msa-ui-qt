#ifndef PROFILEPICTUREMANAGER_H
#define PROFILEPICTUREMANAGER_H

#include <string>
#include <QObject>
#include <QRunnable>
#include <QNetworkAccessManager>

class ProfilePictureManager {

private:
    QNetworkAccessManager networkAccessManagerInstance;

public:
    static ProfilePictureManager instance;

    QNetworkAccessManager& networkManager() { return networkAccessManagerInstance; }

};

class ProfilePictureDownloadTask : public QObject {
    Q_OBJECT

    ProfilePictureManager& manager;
    QString url;

    void doNetworkRequest();

    void onNetworkRequestFinished();

public:
    ProfilePictureDownloadTask(QString url, QObject* parent = nullptr,
                               ProfilePictureManager& manager = ProfilePictureManager::instance)
            : QObject(parent), manager(manager), url(std::move(url)) {}

    void start() {
        doNetworkRequest();
    }

signals:
    void imageAvailable(QImage const& image);

};

#endif //PROFILEPICTUREMANAGER_H
