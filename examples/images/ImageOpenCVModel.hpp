#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/DataModelRegistry>
#include <nodes/NodeDataModel>
#include <opencv2/imgproc/imgproc.hpp>

#include "ImageShowModel.hpp"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class ImageOpenCVModel : public ImageShowModel
{
  Q_OBJECT

public:
  ImageOpenCVModel();

  virtual
  ~ImageOpenCVModel() override {}

public:

  virtual
  QPixmap doOpenCV_Task(const QPixmap &inPixmap) = 0;

  QString
  caption() const override
  { return QString("OpenCV Display"); }

  QString
  name() const override
  { return QString("ImageOpenCVModel"); }

public:

  virtual QString
  modelName() const override
  { return QString("OpenCV Image"); }

  std::shared_ptr<NodeData>
  outData(PortIndex port) override;

  void
  setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

protected:

  bool
  eventFilter(QObject *object, QEvent *event) override;

};
