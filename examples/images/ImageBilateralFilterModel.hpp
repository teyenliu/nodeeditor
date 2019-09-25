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

#include "ImageOpenCVModel.hpp"

class BFObject:public QObject
{
    Q_OBJECT
public:
    BFObject(){}
    ~BFObject(){}

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
class ImageBilateralFilterModel : public ImageShowModel
{
  Q_OBJECT

public:
  ImageBilateralFilterModel();

  virtual
  ~ImageBilateralFilterModel() override
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
  { return QString("BilateralFilter Image Display"); }

  QString
  name() const override
  { return QString("ImageBilateralFilterModel"); }

  // save and restore the data flow graph
  QJsonObject
  save() const override;

  void
  restore(QJsonObject const &p) override;

public:

  virtual QString
  modelName() const override
  { return QString("BilateralFilter Image"); }

  void
  setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

protected:
  QLineEdit * _lineedit1;
  QLineEdit * _lineedit2;
  QLineEdit * _lineedit3;
  QLabel * _llabel1;
  QLabel * _llabel2;
  QLabel * _llabel3;

  BFObject *_cvObj;
  QThread _workerThread;
};
