
#include <cstdio>
#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QDebug>
#include <QtWidgets/QFileDialog>
#include <nodes/DataModelRegistry>

#include "ImageErodeModel.hpp"
#include "PixmapData.hpp"

using namespace cv;

ImageErodeModel::
ImageErodeModel()
  : _cvObj(new ErodeObject())
{
  connect(this, &ImageErodeModel::triggerTask,
              _cvObj, &ErodeObject::runTask);
  connect(_cvObj, &ErodeObject::sendResults,
              this, &ImageErodeModel::handleResults);

  _cvObj->moveToThread(&_workerThread);
  _workerThread.start();
}


void
ImageErodeModel::
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
ImageErodeModel::
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
ErodeObject::
runTask(const QPixmap &frame)
{
    //qDebug()<<"from ErodeObject's sub thread slot:" <<QThread::currentThreadId();
    Mat src = PixmapData::QPixmapToCvMat(frame);
    threshold(src,src,120,255,THRESH_BINARY);
    Mat dst;
    Mat erodeStruct = getStructuringElement(MORPH_RECT,Size(5,5));
    erode(src, dst, erodeStruct);
    emit sendResults(PixmapData::cvMatToQPixmap(dst));
}
