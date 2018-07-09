#ifndef LOGINUIHANDLER_H
#define LOGINUIHANDLER_H

#include <QObject>
#include <QApplication>
#include <simpleipc/server/rpc_handler.h>
#include "loginipcservice.h"

class LoginUIHandler : public QObject {
    Q_OBJECT
private:
    QApplication& app;

    void activateWindow(QWidget* window);

public:
    explicit LoginUIHandler(QApplication& app);

public slots:
    void onStopRequested();

    void pickAccount(QVector<PickAccountEntry> const& accounts,
                     simpleipc::server::rpc_handler::result_handler const& handler);

    void openBrowser(QString const& url, simpleipc::server::rpc_handler::result_handler const& handler);

};

#endif // LOGINUIHANDLER_H
