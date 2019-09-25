
#include <cstdio>
#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QDebug>
#include <QtWidgets/QFileDialog>
#include <nodes/DataModelRegistry>

#include "ImageCannyModel.hpp"
#include "PixmapData.hpp"

using namespace cv;

ImageCannyModel::
ImageCannyModel() :
    _cvObj(new CannyObject())
{
  // connect task object and image model class
  connect(this, &ImageCannyModel::triggerTask,
              _cvObj, &CannyObject::runTask);
  connect(_cvObj, &CannyObject::sendResults,
              this, &ImageCannyModel::handleResults);

  _cvObj->moveToThread(&_workerThread);
  _workerThread.start();
}


void
ImageCannyModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
  _nodeData = nodeData;

  if (_nodeData)
  {
    auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData);
    // Danny Implementation
    if(!d->pixmap().isNull())
    {
      // trigger threading job
      emit triggerTask(d->pixmap(), 50, 150, 3);
    }
  }
  else
  {
    // empty data
    _label->setPixmap(QPixmap());
  }

  Q_EMIT dataUpdated(0);
}


void
ImageCannyModel::
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
CannyObject::
runTask(const QPixmap &frame, int p1, int p2, int p3)
{
    //qDebug()<<"from CannyObject's sub thread slot:" <<QThread::currentThreadId();
    Mat src = PixmapData::QPixmapToCvMat(frame);
    Mat dst;
    //GaussianBlur(src, dst, Size(3,3), 0, 0);
    Canny(src, dst, p1, p2, p3);
    emit sendResults(PixmapData::cvMatToQPixmap(dst));
}
