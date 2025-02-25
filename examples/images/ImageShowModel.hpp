#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <QGridLayout>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <nodes/DataModelRegistry>
#include <nodes/NodeDataModel>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class ImageShowModel : public NodeDataModel
{
  Q_OBJECT

public:
  ImageShowModel();

  virtual
  ~ImageShowModel() override {}

public:

  QString
  caption() const override
  { return QString("Image Display"); }

  QString
  name() const override
  { return QString("ImageShowModel"); }

public:

  virtual QString
  modelName() const
  { return QString("Resulting Image"); }

  unsigned int
  nPorts(PortType portType) const override;

  NodeDataType
  dataType(PortType portType, PortIndex portIndex) const override;

  std::shared_ptr<NodeData>
  outData(PortIndex port) override;

  void
  setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

  QWidget *
  embeddedWidget() override { return _panel; }

  bool
  resizable() const override { return true; }

protected:

  bool
  eventFilter(QObject *object, QEvent *event) override;

  QLabel * _label;
  QString _label_result;
  QGridLayout* _panelLayout;
  QWidget * _panel;

  std::shared_ptr<NodeData> _nodeData;
};
