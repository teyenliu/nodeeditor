#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <QThread>
#include <nodes/DataModelRegistry>
#include <nodes/NodeDataModel>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "ImageShowModel.hpp"

class GrayObject:public QObject
{
    Q_OBJECT
public:
    GrayObject(){}
    ~GrayObject(){}

signals:
    void sendResults(const QPixmap &);

public slots:
    void runTask(const QPixmap &);
};

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class ImageGrayModel : public ImageShowModel
{
  Q_OBJECT

public:
  ImageGrayModel();

  virtual
  ~ImageGrayModel() override
  {
      _workerThread.quit();
      _workerThread.wait();
  }

  // Threading
  public slots:
    void handleResults(const QPixmap &);

  signals:
    void triggerTask(const QPixmap &);

public:

  QString
  caption() const override
  { return QString("Gray Image Display"); }

  QString
  name() const override
  { return QString("ImageGrayModel"); }

public:

  virtual QString
  modelName() const override
  { return QString("Gray Image"); }

  void
  setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;
protected:

  GrayObject *_cvObj;
  QThread _workerThread;
};
