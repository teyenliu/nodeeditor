
#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QDebug>
#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include "ImageTFInfModel.hpp"
#include "PixmapData.hpp"

ImageTFInfModel::
ImageTFInfModel()
{
    const QString graph_fn = QString("/usr/local/tf_libs/resources/my_mnist_model.meta");
    const QString checkpoint_fn = QString("/usr/local/tf_libs/resources/my_mnist_model");
    _tftools = new TFTools(graph_fn, checkpoint_fn);
}

ImageTFInfModel::
~ImageTFInfModel()
{
    if (_tftools != nullptr )
        delete _tftools;
}

std::shared_ptr<NodeData>
ImageTFInfModel::
outData(PortIndex)
{
  return _nodeData;
}

void
ImageTFInfModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
  _nodeData = nodeData;

  if (_nodeData)
  {
    auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData);

    //int w = _label->width();
    //int h = _label->height();

    // Danny Implementation
    if(!d->pixmap().isNull())
    {
        qDebug() << "...convert pixmap to mat for tensorflow inference task";
        cv::Mat _frame = PixmapData::QPixmapToCvMat(d->pixmap());
        // do tensorflow inference task
        qDebug() << "...do tensorflow inference task";
        _label_result = QString::fromStdString(_tftools->doInference(_frame));
        _label->setText(_label_result);
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
ImageTFInfModel::
eventFilter(QObject *object, QEvent *event)
{
  if (object == _label)
  {
    int w = _label->width();
    int h = _label->height();

    if (event->type() == QEvent::Resize)
    {
      auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData);
      if (d)
      {
        _label->setText(_label_result);
        //_label->setPixmap(d->pixmap().scaled(w, h, Qt::KeepAspectRatio));
      }
    }
  }

  return false;
}
