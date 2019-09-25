#include "ImageShowModel.hpp"

#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QDebug>
#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include "PixmapData.hpp"

ImageShowModel::
ImageShowModel()
  : _label(new QLabel("Image will appear here")),
    _panel(new QWidget()),
    _panelLayout(new QGridLayout())
{
  _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
  QFont f = _label->font();
  f.setBold(true);
  f.setItalic(true);
  _label->setFont(f);
  //_label->setFixedSize(200, 200);
  //_label->installEventFilter(this);

  _panelLayout->addWidget(_label, 1, 0);
  _panel->setLayout(_panelLayout);
  _panel->setFixedSize(200, 200);
  _panel->installEventFilter(this);
}

unsigned int
ImageShowModel::
nPorts(PortType portType) const
{
  unsigned int result = 1;

  switch (portType)
  {
    case PortType::In:
      result = 1;
      break;

    case PortType::Out:
      result = 1;

    default:
      break;
  }

  return result;
}


bool
ImageShowModel::
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


NodeDataType
ImageShowModel::
dataType(PortType, PortIndex) const
{
  return PixmapData().type();
}


std::shared_ptr<NodeData>
ImageShowModel::
outData(PortIndex)
{
  return _nodeData;
}


void
ImageShowModel::
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
