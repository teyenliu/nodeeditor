
#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QDebug>
#include <QtWidgets/QFileDialog>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <nodes/DataModelRegistry>

#include "ImageGrayModel.hpp"
#include "PixmapData.hpp"

using namespace cv;

ImageGrayModel::
ImageGrayModel() :
   _cvObj(new GrayObject())
{
  // connect task object and image model class
  connect(this, &ImageGrayModel::triggerTask,
              _cvObj, &GrayObject::runTask);
  connect(_cvObj, &GrayObject::sendResults,
              this, &ImageGrayModel::handleResults);

  _cvObj->moveToThread(&_workerThread);
  _workerThread.start();
}

void
ImageGrayModel::
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
      emit triggerTask(d->pixmap());
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
ImageGrayModel::
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
GrayObject::
runTask(const QPixmap &frame)
{
    //qDebug()<<"from GrayObject's sub thread slot:" <<QThread::currentThreadId();
    Mat src = PixmapData::QPixmapToCvMat(frame);
    Mat dst;
    cv::cvtColor(src, dst, CV_BGR2GRAY);
    emit sendResults(PixmapData::cvMatToQPixmap(dst));
}
