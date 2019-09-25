#include "TextSourceDataModel.hpp"
#include <QDebug>

TextSourceDataModel::
TextSourceDataModel()
  : _lineEdit(new QLineEdit("Default Text"))
{
  connect(_lineEdit, &QLineEdit::textEdited,
          this, &TextSourceDataModel::onTextEdited);
}


unsigned int
TextSourceDataModel::
nPorts(PortType portType) const
{
  unsigned int result = 1;

  switch (portType)
  {
    case PortType::In:
      result = 0;
      break;

    case PortType::Out:
      result = 3;

    default:
      break;
  }

  return result;
}


void
TextSourceDataModel::
onTextEdited(QString const &string)
{
  Q_UNUSED(string);

  for (int i =0; i<3; i++) {
    Q_EMIT dataUpdated(i);
  }
}


NodeDataType
TextSourceDataModel::
dataType(PortType, PortIndex) const
{
  return TextData().type();
}


std::shared_ptr<NodeData>
TextSourceDataModel::
outData(PortIndex port_index)
{
  qDebug() << "output port index is :" << port_index;
  return std::make_shared<TextData>(_lineEdit->text());
}
