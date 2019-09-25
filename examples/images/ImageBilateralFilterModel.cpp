
#include <cstdio>
#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QDebug>
#include <QtWidgets/QFileDialog>
#include <nodes/DataModelRegistry>

#include "ImageBilateralFilterModel.hpp"
#include "PixmapData.hpp"

using namespace cv;

ImageBilateralFilterModel::
ImageBilateralFilterModel()
    : _lineedit1(new QLineEdit()),
      _lineedit2(new QLineEdit()),
      _lineedit3(new QLineEdit()),
      _llabel1(new QLabel()),
      _llabel2(new QLabel()),
      _llabel3(new QLabel()),
    _cvObj(new BFObject())
{
    // connect task object and image model class
    connect(this, &ImageBilateralFilterModel::triggerTask,
                _cvObj, &BFObject::runTask);
    connect(_cvObj, &BFObject::sendResults,
                this, &ImageBilateralFilterModel::handleResults);

    _cvObj->moveToThread(&_workerThread);
    _workerThread.start();

    _llabel1->setText("Param1:");
    _llabel2->setText("Param2:");
    _llabel3->setText("Param3:");

    _lineedit1->setText("5");
    _lineedit2->setText("150");
    _lineedit3->setText("150");

    _panelLayout->addWidget(_llabel1, 2, 0);
    _panelLayout->addWidget(_lineedit1, 2, 1);
    _panelLayout->addWidget(_llabel2, 3, 0);
    _panelLayout->addWidget(_lineedit2, 3, 1);
    _panelLayout->addWidget(_llabel3, 4, 0);
    _panelLayout->addWidget(_lineedit3, 4, 1);
}


void
ImageBilateralFilterModel::
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
        emit triggerTask(d->pixmap(),
                       _lineedit1->text().toInt(),
                       _lineedit2->text().toInt(),
                       _lineedit2->text().toInt());
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
ImageBilateralFilterModel::
save() const
{
  QJsonObject modelJson = NodeDataModel::save();
  if (_lineedit1)
    modelJson["bilateral_filter1"] = QString::number(_lineedit1->text().toInt());
  if (_lineedit2)
    modelJson["bilateral_filter2"] = QString::number(_lineedit2->text().toInt());
  if (_lineedit3)
    modelJson["bilateral_filter3"] = QString::number(_lineedit3->text().toInt());
  return modelJson;
}


void
ImageBilateralFilterModel::
restore(QJsonObject const &p)
{
  QJsonValue v1 = p["bilateral_filter1"];
  if (!v1.isUndefined())
  {
    QString strNum = v1.toString();

    bool ok;
    strNum.toInt(&ok);
    if (ok)
    {
      _lineedit1->setText(strNum);
    }
  }

  QJsonValue v2 = p["bilateral_filter2"];
  if (!v2.isUndefined())
  {
    QString strNum = v2.toString();

    bool ok;
    strNum.toInt(&ok);
    if (ok)
    {
      _lineedit1->setText(strNum);
    }
  }

  QJsonValue v3 = p["bilateral_filter3"];
  if (!v3.isUndefined())
  {
    QString strNum = v3.toString();

    bool ok;
    strNum.toInt(&ok);
    if (ok)
    {
      _lineedit1->setText(strNum);
    }
  }
}

void
ImageBilateralFilterModel::
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
BFObject::
runTask(const QPixmap &frame, int p1, int p2, int p3)
{
    //qDebug()<<"from BFObject's sub thread slot:" <<QThread::currentThreadId();
    Mat src = PixmapData::QPixmapToCvMat(frame);
    Mat dst;
    bilateralFilter(src, dst, p1, p2, p3);
    emit sendResults(PixmapData::cvMatToQPixmap(dst));
}
