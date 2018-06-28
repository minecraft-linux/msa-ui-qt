#include "loginipcservice.h"
#include <QVector>

LoginIPCService::LoginIPCService(const std::string& path, daemon_utils::shutdown_policy shutdown_policy)
        : auto_shutdown_service(path, shutdown_policy) {
    using namespace std::placeholders;
    add_handler("msa/ui/exit", std::bind(&LoginIPCService::handleExit, this));
    add_handler_async("msa/ui/pick_account", std::bind(&LoginIPCService::handlePickAccount, this, _3, _4));
    add_handler_async("msa/ui/open_browser", std::bind(&LoginIPCService::handleOpenBrowser, this, _3, _4));

}

void LoginIPCService::request_stop() {
    emit stopRequested();
    daemon_utils::auto_shutdown_service::request_stop();
}

simpleipc::rpc_json_result LoginIPCService::handleExit() {
    request_stop();
    return simpleipc::rpc_json_result::response(nullptr);
}

void LoginIPCService::handlePickAccount(nlohmann::json const& data, rpc_handler::result_handler const& handler) {
    QVector<PickAccountEntry> entries;
    for (auto const& e : data.at("accounts"))
        entries.push_back({QString::fromStdString(e["cid"]), QString::fromStdString(e["username"])});
    emit pickAccount(std::move(entries), handler);
}

void LoginIPCService::handleOpenBrowser(nlohmann::json const& data, rpc_handler::result_handler const& handler) {
    emit openBrowser(QString::fromStdString(data["url"]), handler);
}