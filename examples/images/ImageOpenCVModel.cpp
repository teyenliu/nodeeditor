#include "ImageOpenCVModel.hpp"

#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QDebug>

#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include "PixmapData.hpp"


ImageOpenCVModel::
ImageOpenCVModel()
{
}

bool
ImageOpenCVModel::
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
          _label->setPixmap(d->pixmap().scaled(w, h, Qt::KeepAspectRatio));
      }
    }
  }

  return false;
}

std::shared_ptr<NodeData>
ImageOpenCVModel::
outData(PortIndex)
{
  return _nodeData;
}

void
ImageOpenCVModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
  _nodeData = nodeData;

  if (_nodeData)
  {
    auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData);
    // Danny Implementation
    if(!d->pixmap().isNull())
    {
      int w = _panel->width();
      int h = _panel->height();
      _label->setPixmap(d->pixmap().scaled(w, h, Qt::KeepAspectRatio));
    }
  }
  else
  {
    _label->setPixmap(QPixmap());
  }
  Q_EMIT dataUpdated(0);
}
