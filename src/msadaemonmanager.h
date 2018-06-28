#ifndef MSADAEMONMANAGER_H
#define MSADAEMONMANAGER_H

#include <msa/client/service_client.h>
#include <QSharedPointer>

class MsaDaemonManager {

public:
    static MsaDaemonManager instance;

    void setPath(std::string const& path) {
        this->path = path;
    }

    QSharedPointer<msa::client::ServiceClient> connectToMsa();

private:
    std::string path;
    QWeakPointer<msa::client::ServiceClient> client;

};

#endif //MSADAEMONMANAGER_H
