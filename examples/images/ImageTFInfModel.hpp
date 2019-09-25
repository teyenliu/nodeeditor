#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/DataModelRegistry>
#include <nodes/NodeDataModel>
#include <opencv2/imgproc/imgproc.hpp>

#include "ImageShowModel.hpp"
#include "PixmapData.hpp"
#include "TFTools.hpp"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;



/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class ImageTFInfModel : public ImageShowModel
{
  Q_OBJECT

public:
  ImageTFInfModel();

  ~ImageTFInfModel() override;

public:

  QString
  caption() const override
  { return QString("TF Inference Display"); }

  QString
  name() const override
  { return QString("ImageTFInfModel"); }

public:

  virtual QString
  modelName() const override
  { return QString("TF Inference Image"); }

  std::shared_ptr<NodeData>
  outData(PortIndex port) override;

  void
  setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

protected:

  bool
  eventFilter(QObject *object, QEvent *event) override;

  TFTools *_tftools;
};
