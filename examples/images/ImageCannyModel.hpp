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

class CannyObject:public QObject
{
    Q_OBJECT
public:
    CannyObject(){}
    ~CannyObject(){}

signals:
    void sendResults(const QPixmap &);

public slots:
    void runTask(const QPixmap &, int, int, int);
};


using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class ImageCannyModel : public ImageShowModel
{
  Q_OBJECT

public:
  ImageCannyModel();

  virtual
  ~ImageCannyModel() override
  {
      _workerThread.quit();
      _workerThread.wait();
  }

// Threading
public slots:
  void handleResults(const QPixmap &);

signals:
  void triggerTask(const QPixmap &, int, int, int);

public:

  QString
  caption() const override
  { return QString("Canny Image Display"); }

  QString
  name() const override
  { return QString("ImageCannyModel"); }

public:

  virtual QString
  modelName() const override
  { return QString("Canny Image"); }

  void
  setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;


protected:

  CannyObject *_cvObj;
  QThread _workerThread;
};
