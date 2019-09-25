#include "ImageFanOutModel.hpp"

#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QDebug>
#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include "PixmapData.hpp"

ImageFanOutModel::
ImageFanOutModel()
  : _label(new QLabel("FanOut:")),
    _lineedit(new QLineEdit("5")),
    _panel(new QWidget()),
    _panelLayout(new QGridLayout())
{
  //_label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
  QFont f = _label->font();
  f.setBold(true);
  f.setItalic(true);
  _label->setFont(f);
  //_label->setFixedSize(200, 200);
  //_label->installEventFilter(this);

  _panelLayout->addWidget(_label, 1, 0);
  _panelLayout->addWidget(_lineedit, 1, 1);
  _panel->setLayout(_panelLayout);
  _panel->setFixedSize(200, 80);
  _panel->installEventFilter(this);
}

unsigned int
ImageFanOutModel::
nPorts(PortType portType) const
{
  unsigned int result = 1;

  switch (portType)
  {
    case PortType::In:
      result = 1;
      break;

    case PortType::Out:
      result = _lineedit->text().toInt();

    default:
      break;
  }

  return result;
}

NodeDataType
ImageFanOutModel::
dataType(PortType, PortIndex) const
{
  return PixmapData().type();
}


std::shared_ptr<NodeData>
ImageFanOutModel::
outData(PortIndex)
{
  return _nodeData;
}

void
ImageFanOutModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
  _nodeData = nodeData;

  if (_nodeData)
  {
    auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData);
    // Danny Implementation
    int divide = _lineedit->text().toInt();
    if(!d->pixmap().isNull())
    {
        // Fan Out Mechanism is implemented here:
        int ret = (_count % divide);
        qDebug() << "_count:" << _count << ", ret:" << ret;
        Q_EMIT dataUpdated(ret);
        _count++;
    }
  }
}

// implement save and restore for data flow graph
QJsonObject
ImageFanOutModel::
save() const
{
  QJsonObject modelJson = NodeDataModel::save();
  if (_lineedit)
    modelJson["fan_out"] = QString::number(_lineedit->text().toInt());
  return modelJson;
}


void
ImageFanOutModel::
restore(QJsonObject const &p)
{
  QJsonValue v = p["fan_out"];

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
