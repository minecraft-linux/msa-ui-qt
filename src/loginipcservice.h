#ifndef LOGINIPCSERVICE_H
#define LOGINIPCSERVICE_H

#include <daemon_utils/auto_shutdown_service.h>
#include <QObject>

class LoginIPCService : public QObject, public daemon_utils::auto_shutdown_service {
    Q_OBJECT

protected:
    void request_stop() override;

public:
    LoginIPCService(const std::string& path, daemon_utils::shutdown_policy shutdown_policy);

    simpleipc::rpc_json_result handleExit();

    void handleOpenBrowser(nlohmann::json const& data, rpc_handler::result_handler const& handler);

signals:
    void stopRequested();

    void openBrowser(QString const& url, simpleipc::server::rpc_handler::result_handler const& handler);

};

#endif // LOGINIPCSERVICE_H