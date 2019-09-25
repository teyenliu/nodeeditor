
#include <QtWidgets>
#include "dragwidget.hpp"

void
IconTextQLabel::mouseMoveEvent(QMouseEvent *event) {
    if(_animation->state()!=QPropertyAnimation::Running)
    {
        _animation->setDuration(200);
        _animation->setKeyValueAt(0,QRect(this->pos().x(),
                                          this->pos().y(),
                                          10,
                                          10));
        _animation->setKeyValueAt(0.2,QRect(this->pos().x(),
                                            this->pos().y(),
                                            20,
                                            20));
        _animation->setKeyValueAt(1,QRect(this->pos().x(),
                                          this->pos().y(),
                                          this->width(),
                                          this->height()));
        //    animation->setStartValue(QRect(10, 10, 100, 30));
        //    animation->setEndValue(QRect(250, 250, 50, 30));
        _animation->start();
    }
}

//! [0]
DragWidget::DragWidget(QWidget *parent, std::shared_ptr<DataModelRegistry> reg)
    : QFrame(parent), _reg(reg)
{
    QString icon_path = "/usr/local/tf_libs/resources/icons/";
    setMinimumSize(100, 100);
    setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
    setAcceptDrops(true);

    iconLayout = new QGridLayout(this);

    int row_num = 2;
    int current_num = 0;

    //debug for printing all registed models
    for (auto const &assoc :_reg->registeredModelsCategoryAssociation()){
        QString file_path = icon_path + assoc.first + ".png";
        qDebug() << "registed model:" << assoc.first << ", flie name:" << file_path;
        IconTextQLabel *toolIcon = new IconTextQLabel(this);
        toolIcon->setIconName(assoc.first);
        toolIcon->setPixmap(QPixmap(file_path).scaled(50,50,Qt::KeepAspectRatio));
        toolIcon->show();
        toolIcon->setAttribute(Qt::WA_DeleteOnClose);
        iconLayout->addWidget(toolIcon,
                              current_num / row_num,
                              current_num % row_num);
        current_num++;
    }

    /*
    IconTextQLabel *boatIcon = new IconTextQLabel(this);
    boatIcon->setIconName("ImageShowModel");
    boatIcon->setPixmap(QPixmap(icon_path + name + ".png"));
    boatIcon->move(10, 10);
    boatIcon->show();
    boatIcon->setAttribute(Qt::WA_DeleteOnClose);

    IconTextQLabel *carIcon = new IconTextQLabel(this);
    carIcon->setIconName("ImageLoaderModel");
    carIcon->setPixmap(QPixmap("/home/liudanny/git/nodeeditor/examples/images/icons/car.png"));
    carIcon->move(100, 10);
    carIcon->show();
    carIcon->setAttribute(Qt::WA_DeleteOnClose);

    IconTextQLabel *houseIcon = new IconTextQLabel(this);
    houseIcon->setIconName("ImageGrayModel");
    houseIcon->setPixmap(QPixmap("/home/liudanny/git/nodeeditor/examples/images/icons/house.png"));
    houseIcon->move(10, 80);
    houseIcon->show();
    houseIcon->setAttribute(Qt::WA_DeleteOnClose);
    */
}
//! [0]

void
DragWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void
DragWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void DragWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QPixmap pixmap;
        QPoint offset;
        QString name;
        dataStream >> pixmap >> offset >> name;

        IconTextQLabel *newIcon = new IconTextQLabel(this);
        newIcon->setPixmap(pixmap);
        newIcon->setIconName(name);
        newIcon->move(event->pos() + offset);
        newIcon->show();
        newIcon->setAttribute(Qt::WA_DeleteOnClose);

        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

//! [1]
void
DragWidget::mousePressEvent(QMouseEvent *event)
{
    IconTextQLabel *child = static_cast<IconTextQLabel*>(childAt(event->pos()));
    if (!child)
        return;

    QPixmap pixmap = *child->pixmap();

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    QString name = child->getIconName();
    // QPixmap + QPoint + QString
    dataStream << pixmap << QPoint(event->pos() - child->pos()) << name;
//! [1]

//! [2]
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-dnditemdata", itemData);
//! [2]

//! [3]
    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);
    drag->setHotSpot(event->pos() - child->pos());
//! [3]

    QPixmap tempPixmap = pixmap;
    QPainter painter;
    painter.begin(&tempPixmap);
    painter.fillRect(pixmap.rect(), QColor(127, 127, 127, 127));
    painter.end();

    child->setPixmap(tempPixmap);

    if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction) {
        child->close();
    } else {
        child->show();
        child->setPixmap(pixmap);
    }
}


std::shared_ptr<DataModelRegistry>
DragWidget::getRegistry(){
    return _reg;
}
