#include "pickaccountwindow.h"
#include <QScrollArea>
#include <QBoxLayout>
#include <QLabel>
#include <QMouseEvent>

PickAccountWindow::PickAccountWindow(QVector<PickAccountEntry> entries, QWidget* parent) : QDialog(parent) {
    setWindowFlag(Qt::Dialog);
    setWindowTitle("Pick a Microsoft Account");
    resize(400, 300);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    QScrollArea* listScrollArea = new QScrollArea(this);
    QWidget* listWidget = new QWidget(this);
    QVBoxLayout* listLayout = new QVBoxLayout(listWidget);

    QPalette pal = palette();
    pal.setColor(QPalette::Background, Qt::white);
    listWidget->setAutoFillBackground(true);
    listWidget->setPalette(pal);

    listWidget->setLayout(listLayout);
    listScrollArea->setWidgetResizable(true);
    listScrollArea->setWidget(listWidget);
    listScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listScrollArea->setFrameStyle(QFrame::NoFrame);
    layout->addWidget(listScrollArea);
    setLayout(layout);

    for (auto& entry : entries) {
        PickAccountRow* row = new PickAccountRow(entry, this);
        row->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
        listLayout->addWidget(row, 0, Qt::AlignTop);
        connect(row, &ClickableWidget::clicked, [this, row]() {
            acceptedCid = row->cid();
            accept();
        });
    }
    {
        AddAccountRow* row = new AddAccountRow(this);
        row->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
        listLayout->addWidget(row, 0, Qt::AlignTop);
        connect(row, &ClickableWidget::clicked, [this, row]() {
            acceptedAddAccount = true;
            accept();
        });
    }
    listLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

void ClickableWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (rect().contains(event->pos()))
        emit clicked();
}

PickAccountRow::PickAccountRow(PickAccountEntry const& entry, QWidget* parent) : ClickableWidget(parent) {
    entryCid = entry.cid;
    QHBoxLayout* layout = new QHBoxLayout(this);
    QLabel* label = new QLabel(this);
    label->setText(entry.username);
    layout->addWidget(label);
    setLayout(layout);
}

AddAccountRow::AddAccountRow(QWidget* parent) : ClickableWidget(parent) {
    QHBoxLayout* layout = new QHBoxLayout(this);
    QLabel* label = new QLabel(this);
    label->setText("Add a new account");
    layout->addWidget(label);
    setLayout(layout);
}