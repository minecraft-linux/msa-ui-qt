#ifndef LOGINUIHANDLER_H
#define LOGINUIHANDLER_H

#include <QObject>
#include <QApplication>
#include <simpleipc/server/rpc_handler.h>

class LoginUIHandler : public QObject {
    Q_OBJECT
private:
    QApplication& app;

public:
    explicit LoginUIHandler(QApplication& app);

public slots:
    void onStopRequested();

    void openBrowser(QString const& url, simpleipc::server::rpc_handler::result_handler const& handler);

};

#endif // LOGINUIHANDLER_H