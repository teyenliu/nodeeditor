
#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QDebug>
#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include "ImageTFLiteInfModel.hpp"
#include "PixmapData.hpp"


using namespace cv;


ImageTFLiteInfModel::
ImageTFLiteInfModel() :
    _liteObj(new LiteObject())
{
    // connect task object and image model class
    connect(this, &ImageTFLiteInfModel::triggerTask,
                _liteObj, &LiteObject::runTask);
    connect(_liteObj, &LiteObject::sendResults,
                this, &ImageTFLiteInfModel::handleResults);

    _liteObj->moveToThread(&_workerThread);
    _workerThread.start();

    // To change font size in QLabel
    QFont f = _label->font();
    f.setPixelSize(20);
    _label->setFont(f);
    _label->setAlignment(Qt::AlignLeft);
}

void
ImageTFLiteInfModel::
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
ImageTFLiteInfModel::
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
ImageTFLiteInfModel::
handleResults(const QString &result)
{
    int w = _panel->width();
    int h = _panel->height();
    _label_result = result;
    _label->setText("Inference:\n" + _label_result);
    _label->setFixedSize(w, h);
    Q_EMIT dataUpdated(0);
}

void
LiteObject::
runTask(const QPixmap &frame)
{
    //qDebug()<<"from LiteObject's sub thread slot:" <<QThread::currentThreadId();
    Mat src = PixmapData::QPixmapToCvMat(frame);
    emit sendResults(QString::fromStdString(_tftools->doInference(src)));
}
