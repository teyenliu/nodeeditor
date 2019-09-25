#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <QThread>
#include <QGridLayout>
#include <nodes/DataModelRegistry>
#include <nodes/NodeDataModel>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "ImageShowModel.hpp"

class ErodeObject:public QObject
{
    Q_OBJECT
public:
    ErodeObject(){}
    ~ErodeObject(){}

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
class ImageErodeModel : public ImageShowModel
{
  Q_OBJECT

public:
  ImageErodeModel();

  virtual
  ~ImageErodeModel() override
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
  { return QString("Erode Image Display"); }

  QString
  name() const override
  { return QString("ImageErodeModel"); }

public:

  virtual QString
  modelName() const override
  { return QString("Erode Image"); }

  void
  setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

protected:

  ErodeObject *_cvObj;
  QThread _workerThread;
};
