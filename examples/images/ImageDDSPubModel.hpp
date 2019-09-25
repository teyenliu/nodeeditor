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

#ifndef __DDS__
#define __DDS__
#include "common/example_export.h"
#include "common/example_utilities.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "dds/HelloWorldImg_DCPS.hpp"
#endif //__DDS__


class DDSPubObject:public QObject
{
    Q_OBJECT
public:
    DDSPubObject();
    ~DDSPubObject(){
    }

    dds::pub::DataWriter<HelloWorldImg::Image> dw;

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
class ImageDDSPubModel : public ImageShowModel
{
  Q_OBJECT

public:
  ImageDDSPubModel();

  virtual
  ~ImageDDSPubModel() override
  {
      _workerThread.quit();
      _workerThread.wait();
  }

signals:
  void triggerTask(const QPixmap &);

public:

  unsigned int
  nPorts(PortType portType) const override;

  QString
  caption() const override
  { return QString("DDS Pub Image Display"); }

  QString
  name() const override
  { return QString("ImageDDSPubModel"); }

  // save and restore the data flow graph
  QJsonObject
  save() const override;

  void
  restore(QJsonObject const &p) override;

public:

  virtual QString
  modelName() const override
  { return QString("DDS Pub Image"); }

  void
  setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

protected:
  bool
  eventFilter(QObject *object, QEvent *event) override;

  QLineEdit *_lineedit1;
  DDSPubObject *_cvObj;
  QThread _workerThread;

};
