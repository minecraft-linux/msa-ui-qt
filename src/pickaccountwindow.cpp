#include "pickaccountwindow.h"
#include <QScrollArea>
#include <QBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QMenu>
#include <QPainter>
#include <QStyleOption>
#include "msadaemonmanager.h"
#include "profilepicturemanager.h"

PickAccountWindow::PickAccountWindow(QVector<PickAccountEntry> entries, QWidget* parent) : QDialog(parent) {
    setWindowFlag(Qt::Dialog);
    setWindowTitle("Pick a Microsoft Account");
    resize(400, 300);

    QPixmap defaultProfilePixmap (":/res/default_profile.svg");

    QVBoxLayout* layout = new QVBoxLayout(this);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    layout->setMargin(0);
#else
    layout->setContentsMargins(0,0,0,0);
#endif
    QScrollArea* listScrollArea = new QScrollArea(this);
    QWidget* listWidget = new QWidget(this);
    QVBoxLayout* listLayout = new QVBoxLayout(listWidget);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    listLayout->setMargin(0);
#else
    listLayout->setContentsMargins(0,0,0,0);
#endif
    listLayout->setSpacing(0);
    listWidget->setProperty("class", "pick-account-list");
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

void PickAccountRow::paintEvent(QPaintEvent* event) {
    QStyleOption opt;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    opt.init(this);
#else
    opt.initFrom(this);
#endif
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);
}

AddAccountRow::AddAccountRow(QWidget* parent) : ClickableWidget(parent) {
    QHBoxLayout* layout = new QHBoxLayout(this);
    QLabel* image = new QLabel(this);
    layout->setSpacing(10);
    image->setContentsMargins(8, 0, 8, 0);
    image->setPixmap(QPixmap(":/res/add_icon.svg"));
    image->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    layout->addWidget(image);
    QLabel* label = new QLabel(this);
    label->setText("Add a new account");
    layout->addWidget(label);
    setLayout(layout);
}

void AddAccountRow::paintEvent(QPaintEvent* event) {
    QStyleOption opt;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    opt.init(this);
#else
    opt.initFrom(this);
#endif
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);
}
