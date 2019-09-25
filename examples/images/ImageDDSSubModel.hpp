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


class DDSSubObject:public QObject
{
    Q_OBJECT
public:
    DDSSubObject(QString const &);
    ~DDSSubObject(){
        dr.close();
    }

    bool isFinished = false;
    dds::sub::DataReader<HelloWorldImg::Image> dr;

signals:
    void sendResults(const QPixmap &);

public slots:
    void runTask();

private:
    QString contentfilter;

};


using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class ImageDDSSubModel : public ImageShowModel
{
  Q_OBJECT

public:
    ImageDDSSubModel();

  virtual
  ~ImageDDSSubModel() override
  {
      _cvObj->isFinished = true;
      _workerThread->quit();
      _workerThread->wait();
      while (_workerThread->isFinished() == false);
      delete _workerThread;
      delete _cvObj;
  }

  unsigned int
  nPorts(PortType portType) const override;

  std::shared_ptr<NodeData>
  outData(PortIndex port) override;

// Threading
public slots:
  void handleResults(const QPixmap &);

signals:
  void triggerTask();


public:

  QString
  caption() const override
  { return QString("DDS Sub Image Display"); }

  QString
  name() const override
  { return QString("ImageDDSSubModel"); }

  // save and restore the data flow graph
  QJsonObject
  save() const override;

  void
  restore(QJsonObject const &p) override;

public:

  virtual QString
  modelName() const override
  { return QString("DDS Sub Image"); }


protected:

  bool
  eventFilter(QObject *object, QEvent *event) override;

  DDSSubObject *_cvObj;
  QThread *_workerThread;
  QPixmap _pixmap;

  QLabel *_label_filter;
  QLineEdit *_lineedit1;
  QLineEdit *_lineedit2;

  //custom dialog
  QDialog *_dialog;
  QFormLayout *_form;
  QDialogButtonBox *_buttonBox;
};
