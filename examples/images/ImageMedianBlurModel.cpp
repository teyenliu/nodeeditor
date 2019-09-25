
#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QDebug>

#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include "ImageMedianBlurModel.hpp"
#include "PixmapData.hpp"

using namespace cv;

ImageMedianBlurModel::
ImageMedianBlurModel()
    : _ksize_label(new QLabel()),
      _lineedit(new QLineEdit()),    
      _cvObj(new MBObject())
{
  // connect task object and image model class
  connect(this, &ImageMedianBlurModel::triggerTask,
              _cvObj, &MBObject::runTask);
  connect(_cvObj, &MBObject::sendResults,
              this, &ImageMedianBlurModel::handleResults);

  _cvObj->moveToThread(&_workerThread);
  _workerThread.start();
  
  _ksize_label->setText("KSize:");
  _lineedit->setText("77");
  _panelLayout->addWidget(_ksize_label, 2, 0);
  _panelLayout->addWidget(_lineedit, 2, 1);
}

void
ImageMedianBlurModel::
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
      emit triggerTask(d->pixmap(), _lineedit->text().toInt());
    }
  }
  else
  {
    // empty data
    _label->setPixmap(QPixmap());
  }

  Q_EMIT dataUpdated(0);
}

// implement save and restore for data flow graph
QJsonObject
ImageMedianBlurModel::
save() const
{
  QJsonObject modelJson = NodeDataModel::save();
  if (_lineedit)
    modelJson["median_blur"] = QString::number(_lineedit->text().toInt());
  return modelJson;
}


void
ImageMedianBlurModel::
restore(QJsonObject const &p)
{
  QJsonValue v = p["median_blur"];

  if (!v.isUndefined())
  {
    QString strNum = v.toString();

    bool ok;
    strNum.toInt(&ok);
    if (ok)
    {
      _lineedit->setText(strNum);
    }
  }
}

void
ImageMedianBlurModel::
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
MBObject::
runTask(const QPixmap &frame, int p1)
{
    //qDebug()<<"from CannyObject's sub thread slot:" <<QThread::currentThreadId();
    Mat src = PixmapData::QPixmapToCvMat(frame);
    Mat dst;
    medianBlur(src, dst, p1);
    emit sendResults(PixmapData::cvMatToQPixmap(dst));
}
