#include <QApplication>
#include <QTextStream>
#include <QtCore/QCommandLineParser>
#include <QFile>
#include "webloginwindow.h"
#include "loginipcservice.h"
#include "loginuihandler.h"
#include "msadaemonmanager.h"
#ifdef __APPLE__
#include "macosutil.h"
#endif

int main(int argc, char *argv[]) {
    qRegisterMetaType<simpleipc::server::rpc_handler::result_handler>("simpleipc::server::rpc_handler::result_handler");
    qRegisterMetaType<QVector<PickAccountEntry>>("QVector<PickAccountEntry>");

    QApplication app(argc, argv);
#ifdef __APPLE__
    MacOSUtil::hideDockIcon();
#endif
    {
        QFile file (":/res/style.css");
        file.open(QIODevice::ReadOnly);
        app.setStyleSheet(file.readAll());
    }
    app.setQuitOnLastWindowClosed(false);
    QCommandLineParser parser;
    parser.addHelpOption();
    QCommandLineOption pathOption ({"p", "path"}, "Service path", "path");
    QCommandLineOption autoExitOption ({"x", "auto-exit"}, "Auto-exit the service");
    QCommandLineOption msaPathOption ({"m", "msa-path"}, "Path to the MSA service", "path");
    parser.addOption(pathOption);
    parser.addOption(autoExitOption);
    parser.addOption(msaPathOption);
    parser.process(app);
    MsaDaemonManager::instance.setPath(parser.value(msaPathOption).toStdString());
    std::string path = parser.value(pathOption).toStdString();
    auto mode = parser.isSet(autoExitOption) ? daemon_utils::shutdown_policy::no_connections
                                             : daemon_utils::shutdown_policy::never;
    LoginIPCService service(path, mode);
    LoginUIHandler uiHandler (app);
    QObject::connect(&service, &LoginIPCService::stopRequested, &uiHandler, &LoginUIHandler::onStopRequested);
    QObject::connect(&service, &LoginIPCService::pickAccount, &uiHandler, &LoginUIHandler::pickAccount);
    QObject::connect(&service, &LoginIPCService::openBrowser, &uiHandler, &LoginUIHandler::openBrowser);
    return app.exec();
}
