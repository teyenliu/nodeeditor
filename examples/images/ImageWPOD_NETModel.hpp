#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <QThread>
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


class WPODObject:public QObject
{
    Q_OBJECT
public:
    WPODObject()
    {
        const QString tflite_file = QString("/usr/local/tf_libs/resources/wpod_model.pb");
        _tftools = new TFTools(tflite_file);
    }
    ~WPODObject()
    {
        if (_tftools != nullptr )
            delete _tftools;
    }

signals:
    void sendResults(const QPixmap &);

public slots:
    void runTask(const QPixmap &);

private:
    TFTools *_tftools;
};


/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class ImageWPOD_NETModel : public ImageShowModel
{
  Q_OBJECT

public:
  ImageWPOD_NETModel();

  ~ImageWPOD_NETModel() override
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
  { return QString("WPOD Inference Display"); }

  QString
  name() const override
  { return QString("ImageWPOD_NETModel"); }

public:

  virtual QString
  modelName() const override
  { return QString("WPOD Inference Image"); }

  void
  setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

protected:

  bool
  eventFilter(QObject *object, QEvent *event) override;

private:
  WPODObject *_liteObj;
  QThread _workerThread;
};
