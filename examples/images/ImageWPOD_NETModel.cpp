
#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QDebug>
#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include "ImageWPOD_NETModel.hpp"
#include "PixmapData.hpp"


using namespace cv;


ImageWPOD_NETModel::
ImageWPOD_NETModel() :
    _liteObj(new WPODObject())
{
    // connect task object and image model class
    connect(this, &ImageWPOD_NETModel::triggerTask,
                _liteObj, &WPODObject::runTask);
    connect(_liteObj, &WPODObject::sendResults,
                this, &ImageWPOD_NETModel::handleResults);

    _liteObj->moveToThread(&_workerThread);
    _workerThread.start();
}

void
ImageWPOD_NETModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
  _nodeData = nodeData;

  if (_nodeData)
  {
    auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData);

    // Danny Implementation
    if(!d->pixmap().isNull())
    {
        // trigger threading job to do tensorflow inference task
        emit triggerTask(d->pixmap());
    }
  }
  else
  {
    _label->setPixmap(QPixmap());
    _label->setText("No image for inferencing");
  }

  Q_EMIT dataUpdated(0);
}

bool
ImageWPOD_NETModel::
eventFilter(QObject *object, QEvent *event)
{
  if (object == _panel)
  {
      int w = _panel->width();
      int h = _panel->height();

    if (event->type() == QEvent::Resize)
    {
      auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData);
      if (d)
      {
        _label->setText(_label_result);
        _label->setFixedSize(w, h);
      }
    }
  }

  return false;
}


void
ImageWPOD_NETModel::
handleResults(const QPixmap &pixmap)
{
    int w = _panel->width();
    int h = _panel->height();
    auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData);
    if(!pixmap.isNull())
    {
        d->setPixmap(pixmap);
    }
    _label->setPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio));
    Q_EMIT dataUpdated(0);
}

void
WPODObject::
runTask(const QPixmap &frame)
{
    //qDebug()<<"from LiteObject's sub thread slot:" <<QThread::currentThreadId();
    Mat dst = _tftools->doFrozenInference(PixmapData::QPixmapToCvMat(frame));
    emit sendResults(PixmapData::cvMatToQPixmap(dst));
}
