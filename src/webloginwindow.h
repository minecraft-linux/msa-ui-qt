#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include <QMap>

class QStackedLayout;
class QNetworkCookie;
class QWebEngineView;

class WebLoginWindow : public QDialog {
    Q_OBJECT
private:
    QStackedLayout* stacked;
    QWidget* loadingIndicatorCtr;
    QWebEngineView* webView;
    QMap<QString, QString> propertyMap;

    void setupWebBrowser();
    void injectWebScripts();

    void onLoadFinished(bool ok);

public:
    explicit WebLoginWindow(QUrl url, QWidget *parent = nullptr);

    QMap<QString, QString> const& properties() const { return propertyMap; }

    void setProperty(QString const& name, QString const& value);

    void onFinalNext();

    void onFinalBack();
};

class WebLoginWindowApi : public QObject {
    Q_OBJECT

private:
    WebLoginWindow* window;

public:
    WebLoginWindowApi(WebLoginWindow* window, QObject *parent) : QObject(parent), window(window) {}

public slots:
    void setProperty(QString const& name, QString const& value) { window->setProperty(name, value); }

    void onFinalNext() { window->onFinalNext(); }

    void onFinalBack() { window->onFinalBack(); }

};

#endif // LOGINWINDOW_H