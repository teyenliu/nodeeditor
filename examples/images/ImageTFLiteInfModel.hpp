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
#include "TFLiteTools.hpp"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;


class LiteObject:public QObject
{
    Q_OBJECT
public:
    LiteObject()
    {
        const QString tflite_file = QString("/usr/local/tf_libs/resources/inception_v3.tflite");
        const QString label_file = QString("/usr/local/tf_libs/resources/imagenet_slim_labels.txt");
        _tftools = new TFLiteTools(tflite_file, label_file);
    }
    ~LiteObject()
    {
        if (_tftools != nullptr )
            delete _tftools;
    }

signals:
    void sendResults(const QString &);

public slots:
    void runTask(const QPixmap &);

private:
    TFLiteTools *_tftools;
};


/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class ImageTFLiteInfModel : public ImageShowModel
{
  Q_OBJECT

public:
  ImageTFLiteInfModel();

  ~ImageTFLiteInfModel() override
  {
      _workerThread.quit();
      _workerThread.wait();
  }

// Threading
public slots:
  void handleResults(const QString &);

signals:
  void triggerTask(const QPixmap &);

public:

  QString
  caption() const override
  { return QString("TFLite Inference Display"); }

  QString
  name() const override
  { return QString("ImageTFLiteInfModel"); }

public:

  virtual QString
  modelName() const override
  { return QString("TFLite Inference Image"); }

  void
  setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

protected:

  bool
  eventFilter(QObject *object, QEvent *event) override;

  LiteObject *_liteObj;
  QThread _workerThread;
};
