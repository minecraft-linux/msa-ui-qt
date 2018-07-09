#include "loginuihandler.h"
#include "webloginwindow.h"
#include "pickaccountwindow.h"
#include <QUrl>
#ifdef __APPLE__
#include "macosutil.h"
#endif

LoginUIHandler::LoginUIHandler(QApplication& app) : app(app) {
}

void LoginUIHandler::onStopRequested() {
    app.quit();
}

void LoginUIHandler::activateWindow(QWidget* window) {
    window->raise();
    window->activateWindow();
#ifdef __APPLE__
    MacOSUtil::activateApplication();
#endif
}

void LoginUIHandler::pickAccount(QVector<PickAccountEntry> const& accounts,
                                 simpleipc::server::rpc_handler::result_handler const& handler) {
    PickAccountWindow* window = new PickAccountWindow(accounts);
    connect(window, &QDialog::finished, [window, handler](int result) {
        if (result == QDialog::Accepted) {
            if (window->shouldAddNewAccount())
                handler(simpleipc::rpc_json_result::response({{"add_account", true}}));
            else
                handler(simpleipc::rpc_json_result::response({{"cid", window->cid().toStdString()}}));
        } else {
            handler(simpleipc::rpc_json_result::error(-501, "Operation cancelled by user"));
        }
        window->deleteLater();
    });
    window->open();
    activateWindow(window);
}

void LoginUIHandler::openBrowser(QString const& url, simpleipc::server::rpc_handler::result_handler const& handler) {
    WebLoginWindow* window = new WebLoginWindow(QUrl(url));
    connect(window, &QDialog::finished, [window, handler](int result) {
        if (result == QDialog::Accepted) {
            nlohmann::json res;
            auto& prop = res["properties"] = nlohmann::json::object();
            for (auto it = window->properties().begin(); it != window->properties().end(); it++)
                prop[it.key().toStdString()] = it.value().toStdString();
            handler(simpleipc::rpc_json_result::response(res));
        } else {
            handler(simpleipc::rpc_json_result::error(-501, "Operation cancelled by user"));
        }
        window->deleteLater();
    });
    window->open();
    activateWindow(window);
}
