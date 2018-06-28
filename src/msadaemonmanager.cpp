#include "msadaemonmanager.h"

MsaDaemonManager MsaDaemonManager::instance;

QSharedPointer<msa::client::ServiceClient> MsaDaemonManager::connectToMsa() {
    auto client = this->client.lock();
    if (client)
        return client;
    if (path.empty())
        return nullptr;
    try {
        client = QSharedPointer<msa::client::ServiceClient>::create(path);
    } catch (std::exception& e) {
        return nullptr;
    }
    this->client = client;
    return client;
}