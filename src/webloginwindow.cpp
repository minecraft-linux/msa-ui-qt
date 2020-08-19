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
#include <QWebEngineUrlScheme>
#include <QWebEngineUrlSchemeHandler>
#include <QWebEngineUrlRequestJob>

#include "materialbusyindicator.h"

class XalSchemeHandler : public QWebEngineUrlSchemeHandler
{
public:
    WebLoginWindow *loginwindow;

    XalSchemeHandler(WebLoginWindow *parent) : QWebEngineUrlSchemeHandler(parent){
        loginwindow = parent;
    }

    void requestStarted(QWebEngineUrlRequestJob *request)
    {
        loginwindow->setProperty("endurl", request->requestUrl().toString());
        loginwindow->onFinalNext();
    }
};

WebLoginWindow::WebLoginWindow(QUrl url, QUrl endurl, QWidget *parent) : QDialog(parent), endurl(endurl) {
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
    webView->setUrl(url);

    setLayout(stacked);
}

void WebLoginWindow::setupWebBrowser() {
    if (endurl.isEmpty()) {
        connect(webView, &QWebEngineView::loadFinished, this, &WebLoginWindow::onLoadFinished);
        injectWebScripts();
    } else {
        XalSchemeHandler *handler = new XalSchemeHandler(this);
        scheme = endurl.scheme().toStdString();
        QWebEngineUrlScheme scheme_(scheme.c_str());
        scheme_.setSyntax(QWebEngineUrlScheme::Syntax::Path);
        scheme_.setFlags(QWebEngineUrlScheme::SecureScheme);
        QWebEngineUrlScheme::registerScheme(scheme_);
        QWebEngineProfile::defaultProfile()->installUrlSchemeHandler(scheme.c_str(), handler);
        // Note: This page doesn't load if done on loadFinished
        stacked->setCurrentWidget(webView);
    }

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
    accept();
}

void WebLoginWindow::onFinalBack() {
    reject();
}