#include "webloginwindow.h"
#include <QDebug>
#include <QFile>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QWebEngineView>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QWebEngineCookieStore>
#include <QWebEngineProfile>
#include <QWebChannel>
#include "materialbusyindicator.h"

const char* const WebLoginWindow::DEFAULT_URL = "https://login.live.com/ppsecure/InlineConnect.srf?id=80604&platform=android2.1.0504.0524";

WebLoginWindow::WebLoginWindow(QWidget *parent) : QDialog(parent) {
    setWindowFlag(Qt::Dialog);
    setWindowTitle("Microsoft Account Sign-In");
    resize(480, 640);

    stacked = new QStackedLayout(this);

    loadingIndicatorCtr = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(loadingIndicatorCtr);
    MaterialBusyIndicator* loadingIndicator = new MaterialBusyIndicator(loadingIndicatorCtr);
    loadingIndicator->setFixedSize(48, 48);
    layout->addWidget(loadingIndicator, 0, Qt::AlignCenter);
    stacked->addWidget(loadingIndicatorCtr);

    webView = new QWebEngineView(this);
    stacked->addWidget(webView);
    setupWebBrowser();
    webView->setUrl(QUrl(DEFAULT_URL));

    setLayout(stacked);
}

void WebLoginWindow::setupWebBrowser() {
    connect(webView, &QWebEngineView::loadFinished, this, &WebLoginWindow::onLoadFinished);

    injectWebScripts();

    QWebChannel* channel = new QWebChannel(webView);
    WebLoginWindowApi* api = new WebLoginWindowApi(this, webView);
    channel->registerObject(QStringLiteral("loginWindow"), api);
    webView->page()->setWebChannel(channel);

    QWebEngineCookieStore* cookies = webView->page()->profile()->cookieStore();
    cookies->deleteAllCookies();
}

void WebLoginWindow::injectWebScripts() {
    QWebEngineScript script;
    script.setInjectionPoint(QWebEngineScript::DocumentCreation);
    script.setWorldId(QWebEngineScript::MainWorld);
    script.setName("SignInScript");
    QString source;
    {
        QFile file;
        file.setFileName(":/qtwebchannel/qwebchannel.js");
        file.open(QIODevice::ReadOnly);
        source += file.readAll();
    }
    {
        QFile file;
        file.setFileName(":/src/injectjs.js");
        file.open(QIODevice::ReadOnly);
        source += file.readAll();
    }
    script.setSourceCode(source);
    webView->page()->scripts().insert(script);
}

void WebLoginWindow::onLoadFinished(bool ok) {
    stacked->setCurrentWidget(webView);
}

void WebLoginWindow::setProperty(QString const& name, QString const& value) {
    propertyMap[name] = value;
}

void WebLoginWindow::onFinalNext() {
    succeeded = true;
    close();
}

void WebLoginWindow::onFinalBack() {
    succeeded = false;
    close();
}