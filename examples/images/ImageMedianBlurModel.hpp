#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <QLineEdit>
#include <QThread>
#include <nodes/DataModelRegistry>
#include <nodes/NodeDataModel>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "ImageShowModel.hpp"

class MBObject:public QObject
{
    Q_OBJECT
public:
    MBObject(){}
    ~MBObject(){}

signals:
    void sendResults(const QPixmap &);

public slots:
    void runTask(const QPixmap &, int);
};

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class ImageMedianBlurModel : public ImageShowModel
{
  Q_OBJECT

public:
  ImageMedianBlurModel();

  virtual
  ~ImageMedianBlurModel() override 
  {
	  _workerThread.quit();
      _workerThread.wait();
  }

// Threading
public slots:
  void handleResults(const QPixmap &);

signals:
  void triggerTask(const QPixmap &, int);
  
public:

  QString
  caption() const override
  { return QString("MedianBlur Image Display"); }

  QString
  name() const override
  { return QString("ImageMedianBlurModel"); }

  // save and restore the data flow graph
  QJsonObject
  save() const override;

  void
  restore(QJsonObject const &p) override;

public:

  virtual QString
  modelName() const override
  { return QString("MedianBlur Image"); }
  
  void
  setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

protected:
  QLabel * _ksize_label;
  QLineEdit * _lineedit;
  MBObject *_cvObj;
  QThread _workerThread;
};
