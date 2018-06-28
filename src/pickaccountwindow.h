#ifndef PICKACCOUNTWINDOW_H
#define PICKACCOUNTWINDOW_H

#include <QDialog>
#include "loginipcservice.h"

class PickAccountWindow : public QDialog {
    Q_OBJECT
private:
    QString acceptedCid;
    bool acceptedAddAccount = false;

public:
    PickAccountWindow(QVector<PickAccountEntry> entries, QWidget *parent = nullptr);

    QString const& cid() const { return acceptedCid; }

    bool shouldAddNewAccount() const { return acceptedAddAccount; }
};

class ClickableWidget : public QWidget {
    Q_OBJECT
public:
    ClickableWidget(QWidget* parent = nullptr) : QWidget(parent) {}

    void mousePressEvent(QMouseEvent* event) override {}

    void mouseReleaseEvent(QMouseEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override {}

signals:
    void clicked();

};


class PickAccountRow : public ClickableWidget {
Q_OBJECT

private:
    QString entryCid;

public:
    PickAccountRow(PickAccountEntry const& entry, QWidget* parent = nullptr);

    QString const& cid() const { return entryCid; }


};

class AddAccountRow : public ClickableWidget {
Q_OBJECT

public:
    AddAccountRow(QWidget* parent = nullptr);

};

#endif //PICKACCOUNTWINDOW_H
