#include "pickaccountwindow.h"
#include <QScrollArea>
#include <QBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QMenu>
#include "msadaemonmanager.h"
#include "profilepicturemanager.h"

PickAccountWindow::PickAccountWindow(QVector<PickAccountEntry> entries, QWidget* parent) : QDialog(parent) {
    setWindowFlag(Qt::Dialog);
    setWindowTitle("Pick a Microsoft Account");
    resize(400, 300);

    QPixmap defaultProfilePixmap (":/res/default_profile.svg");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    QScrollArea* listScrollArea = new QScrollArea(this);
    QWidget* listWidget = new QWidget(this);
    QVBoxLayout* listLayout = new QVBoxLayout(listWidget);
    listLayout->setMargin(0);

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
        PickAccountRow* row = new PickAccountRow(entry, defaultProfilePixmap, this);
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
    if (rect().contains(event->pos()) && event->button() == Qt::LeftButton)
        emit clicked();
}

PickAccountRow::PickAccountRow(PickAccountEntry const& entry, QPixmap& fallbackIcon, QWidget* parent)
        : ClickableWidget(parent) {
    entryCid = entry.cid;
    QHBoxLayout* layout = new QHBoxLayout(this);
    image = new QLabel(this);
    layout->setSpacing(10);
    image->setPixmap(fallbackIcon);
    image->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    layout->addWidget(image);
    QLabel* label = new QLabel(this);
    label->setText(entry.username);
    layout->addWidget(label);
    setLayout(layout);

    QString imageUrl = "https://storage.live.com/users/0x" + entry.cid + "/myprofile/expressionprofile/profilephoto:UserTileStatic";
    ProfilePictureDownloadTask* task = new ProfilePictureDownloadTask(entry.cid, imageUrl, this);
    connect(task, &ProfilePictureDownloadTask::imageAvailable, this, &PickAccountRow::setImage);
    task->start();
}

void PickAccountRow::setImage(QImage const& image) {
    this->image->setPixmap(QPixmap::fromImage(image));
}

void PickAccountRow::contextMenuEvent(QContextMenuEvent* event) {
    QMenu menu(this);
    QAction removeAction("Remove");
    connect(&removeAction, &QAction::triggered, this, &PickAccountRow::remove);
    menu.addAction(&removeAction);
    menu.exec(event->globalPos());
}

void PickAccountRow::remove() {
    auto msa = MsaDaemonManager::instance.connectToMsa();
    if (msa) {
        auto res = msa->removeAccount(cid().toStdString()).call();
        if (res.success())
            deleteLater();
    }
}

AddAccountRow::AddAccountRow(QWidget* parent) : ClickableWidget(parent) {
    QHBoxLayout* layout = new QHBoxLayout(this);
    QLabel* label = new QLabel(this);
    label->setText("Add a new account");
    layout->addWidget(label);
    setLayout(layout);
}